#include <string.h>

#include "bcompiler.h"
#include "common.h"
#include "lexer.h"
#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif // DEBUG_PRINT_CODE

static void
errorAt(Parser *parser, Token token, const char *message)
{
    if (parser->panicMode) return;
    parser->panicMode = true;

    fprintf(stderr, "[ERROR] on [Ln %d]", token.line);
    if (token.type == TK_EOF) {
        fprintf(stderr, " at end");
    } else if (token.type == TK_ERROR) {
        // Nothing
    } else {
        fprintf(stderr, " at '%.*s'", token.length, token.start);
    }
    fprintf(stderr, ": %s\n", message);

    parser->hadError = true;
}

static void
error(Parser *parser, const char *message)
{
    errorAt(parser, parser->previous, message);
}

static void
errorAtCurrent(Parser *parser, const char *message)
{
    errorAt(parser, parser->current, message);
}

static void
advance(Parser *parser)
{
    parser->previous = parser->current;

    for (;;) {
        parser->current = scanToken(&parser->lexer);
        if (parser->current.type != TK_ERROR) break;

        errorAtCurrent(parser, parser->current.start);
    }
}

static void
consume(Compiler *compiler, TokenType type, const char *message)
{
    if (compiler->parser->current.type == type) {
        advance(compiler->parser);
        return;
    }

    errorAtCurrent(compiler->parser, message);
}

static bool
check(Compiler *compiler, TokenType type)
{
    return compiler->parser->current.type == type;
}

static bool
match(Compiler *compiler, TokenType type)
{
    if (!check(compiler, type)) return false;
    advance(compiler->parser);
    return true;
}

static Chunk *
currentChunk(Compiler *compiler)
{
    return compiler->compiling;
}

static void
emitByte(Compiler *compiler, uint8_t byte)
{
    appendChunk(
        currentChunk(compiler),
        byte,
        compiler->parser->previous.line
    );
}

static void
emitBytes(Compiler *compiler, uint8_t byte, uint8_t byte2)
{
    emitByte(compiler, byte);
    emitByte(compiler, byte2);
}

static void
emitLoop(Compiler *compiler, int loopStart)
{
    emitByte(compiler, OP_LOOP);

    int offset = currentChunk(compiler)->count - loopStart + 2;
    if (offset > UINT16_MAX) error(compiler->parser, "Loop body is too large.");

    emitByte(compiler, (offset >> 8) & 0xff);
    emitByte(compiler, offset & 0xff);
}

static int
emitJump(Compiler *compiler, uint8_t instruction)
{
    emitByte(compiler, instruction);
    emitByte(compiler, 0xff);
    emitByte(compiler, 0xff);

    return currentChunk(compiler)->count - 2;
}

static void
emitReturn(Compiler *compiler)
{
    emitByte(compiler, OP_RETURN);
}

static uint8_t
makeConstant(Compiler *compiler, Value value)
{
    int constant = addConstant(currentChunk(compiler), value);
    if (constant > UINT8_MAX) {
        error(compiler->parser, "Too many constants in one chunk!");
        return 0;
    }

    return (uint8_t)constant;
}

static void
emitConstant(Compiler *compiler, Value value)
{
    emitBytes(compiler, OP_CONSTANT, makeConstant(compiler, value));
}

static void
patchJump(Compiler *compiler, int offset)
{
    // -2 to adjust for the jump offset in the bytecode chunk
    int jump = currentChunk(compiler)->count - offset - 2;

    if (jump > UINT16_MAX) {
        error(compiler->parser, "Too much code to jump over.");
    }

    currentChunk(compiler)->code[offset] = (jump >> 8) & 0xff;
    currentChunk(compiler)->code[offset + 1] = jump & 0xff;
}

static void
initCompiler(Parser *parser, Compiler *compiler, Chunk *chunk)
{
    compiler->parser = parser;
    compiler->compiling = chunk;
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
}

static void
endCompiler(Compiler *compiler)
{
    emitReturn(compiler);

#ifdef DEBUG_PRINT_CODE
    if (!compiler->parser->hadError) {
        disassembleChunk(currentChunk(compiler), "Debug Code");
    }
#endif // DEBUG_PRINT_CODE

}

static void
beginScope(Compiler *compiler)
{
    compiler->scopeDepth++;
}

static void
endScope(Compiler *compiler)
{
    compiler->scopeDepth--;

    while (compiler->localCount > 0 &&
           compiler->locals[compiler->localCount - 1].depth >
            compiler->scopeDepth) {

        emitByte(compiler, OP_POP);
        compiler->localCount--;
    }
}

static void
expression(Compiler *compiler);

static void
declaration(Compiler *compiler);

static void
statement(Compiler *compiler);

static ParseRule *
getRule(TokenType type);

static void
parsePrecedence(Compiler *compiler, Precedence precedence);

static uint8_t
identifierConstant(Compiler *compiler, Token *name);

static int
resolveLocal(Compiler *compiler, Token *name);

static void
addLocal(Compiler *compiler, Token name);

static uint8_t
parseVariable(Compiler *compiler, const char *message);

static void
defineVariable(Compiler *compiler, uint8_t global);

static void
binary(Compiler *compiler, bool canAssign)
{
    TokenType opType = compiler->parser->previous.type;
    ParseRule *rule = getRule(opType);
    parsePrecedence(compiler, (Precedence)(rule->precedence + 1));

    switch (opType) {
        case TK_BANGEQ:     emitBytes(compiler, OP_EQUAL, OP_NOT);      break;
        case TK_EQEQ:       emitByte(compiler, OP_EQUAL);               break;
        case TK_GREATER:    emitByte(compiler, OP_GREATER);             break;
        case TK_GTEQ:       emitBytes(compiler, OP_LESS, OP_NOT);       break;
        case TK_LESS:       emitByte(compiler, OP_LESS);                break;
        case TK_LTEQ:       emitBytes(compiler, OP_GREATER, OP_NOT);    break;
        case TK_PLUS:       emitByte(compiler, OP_ADD);                 break;
        case TK_MINUS:      emitByte(compiler, OP_SUBTRACT);            break;
        case TK_STAR:       emitByte(compiler, OP_MULTIPLY);            break;
        case TK_SLASH:      emitByte(compiler, OP_DIVIDE);              break;
        case TK_MODULUS:    emitByte(compiler, OP_MODULUS);             break;
        case TK_POWER:      emitByte(compiler, OP_POWER);               break;
        case TK_BAND:       emitByte(compiler, OP_BAND);                break;
        case TK_BOR:        emitByte(compiler, OP_BOR);                 break;
        case TK_BXOR:       emitByte(compiler, OP_BXOR);                break;
        case TK_SHL:        emitByte(compiler, OP_SHL);                 break;
        case TK_SHR:        emitByte(compiler, OP_SHR);                 break;
        default:            return; // Unreachable
    }
}

static void
literal(Compiler *compiler, bool canAssign)
{
    switch (compiler->parser->previous.type) {
        case TK_FALSE:      emitByte(compiler, OP_FALSE); break;
        case TK_NULL:       emitByte(compiler, OP_NULL);  break;
        case TK_TRUE:       emitByte(compiler, OP_TRUE);  break;
        default:            return; // Unreachable
    }
}

static void
grouping(Compiler *compiler, bool canAssign)
{
    expression(compiler);
    consume(compiler, TK_RPAREN, "Expected ')' after expression.");
}

static void
number(Compiler *compiler, bool canAssign)
{
    double value = strtod(compiler->parser->previous.start, NULL);
    emitConstant(compiler, NUMBER_VAL(value));
}

static int
escapeSequence(Parser *parser, char *string, int length)
{
    for (int i = 0; i < length - 1; i++) {
        if (string[i] == '\\') {
            switch (string[i + 1]) {
                case '\\':  string[i + 1] = '\\';   break;
                case 'b':   string[i + 1] = '\b';   break;
                case 'n':   string[i + 1] = '\n';   break;
                case 'r':   string[i + 1] = '\r';   break;
                case 't':   string[i + 1] = '\t';   break;
                case 'v':   string[i + 1] = '\v';   break;
                case '"':   break;
                default:    continue;
            }
            memmove(&string[i], &string[i + 1], length - i);
            length -= 1;
        }
    }

    return length;
}

static Value
parseString(Compiler *compiler, bool canAssign)
{
    Parser *parser = compiler->parser;
    int strLen = parser->previous.length - 2;
    char *string = ALLOCATE(char, strLen + 1);

    memcpy(string, parser->previous.start + 1, strLen);
    int length = escapeSequence(parser, string, strLen);

    if (length != strLen) {
        string = GROW_ARRAY(char, string, strLen + 1, length + 1);
    }
    string[length] = '\0';
    return OBJ_VAL(takeString(parser->vm, string, length));
}

static void
string(Compiler *compiler, bool canAssign)
{
    // emitConstant(compiler, OBJ_VAL(
    //     copyString(compiler->parser->vm, 
    //                compiler->parser->previous.start + 1,
    //                compiler->parser->previous.length - 2)));
    emitConstant(compiler, parseString(compiler, canAssign));
}

static void
namedVariable(Compiler *compiler, Token name, bool canAssign)
{
    uint8_t getOp, setOp;
    int arg = resolveLocal(compiler, &name);

    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else {
        arg = identifierConstant(compiler, &name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign && match(compiler, TK_EQ)) {
        expression(compiler);
        emitBytes(compiler, setOp, (uint8_t)arg);
    } else {
        emitBytes(compiler, getOp, (uint8_t)arg);
    }
}

static void
variable(Compiler *compiler, bool canAssign)
{
    namedVariable(compiler, compiler->parser->previous, canAssign);
}

static void
unary(Compiler *compiler, bool canAssign)
{
    TokenType opType = compiler->parser->previous.type;
    parsePrecedence(compiler, PREC_UNARY);

    switch (opType) {
        case TK_MINUS:  emitByte(compiler, OP_NEGATE);  break;
        case TK_BANG:   emitByte(compiler, OP_NOT);     break;
        default:        return; // Unreachable
    }
}

static void
expression(Compiler *compiler)
{
    parsePrecedence(compiler, PREC_ASSIGNMENT);
}

static void
block(Compiler *compiler)
{
    while (!check(compiler, TK_RBRACE) && !check(compiler, TK_EOF)) {
        declaration(compiler);
    }

    consume(compiler, TK_RBRACE, "Expected '}' after block.");
}

static void
varDeclaration(Compiler *compiler)
{
    uint8_t global = parseVariable(compiler, "Expected variable name.");

    if (match(compiler, TK_EQ)) {
        expression(compiler);
    } else {
        emitByte(compiler, OP_NULL);
    }

    consume(compiler, TK_SEMICOLON, "Expected ';' after variable declaration.");
    defineVariable(compiler, global);
}

static void
expressionStatement(Compiler *compiler)
{
    expression(compiler);
    consume(compiler, TK_SEMICOLON, "Expected ';' after expression or loop initializer.");
    emitByte(compiler, OP_POP);
}

static void
forStatement(Compiler *compiler)
{
    beginScope(compiler);
    consume(compiler, TK_LPAREN, "Expected '(' after 'for'.");
    if (match(compiler, TK_SEMICOLON)) {
        // No initializer
    } else if (match(compiler, TK_VAR)) {
        varDeclaration(compiler);
    } else {
        expressionStatement(compiler);
    }

    int loopStart = currentChunk(compiler)->count;
    int exitJump = -1;
    if (!match(compiler, TK_SEMICOLON)) {
        expression(compiler);
        consume(compiler, TK_SEMICOLON, "Expected ';' after loop condition.");

        // Jump out of the loop of the condition is false
        exitJump = emitJump(compiler, OP_JUMP_FALSE);
        emitByte(compiler, OP_POP); // Condition
    }

    if (!match(compiler, TK_RPAREN)) {
        int bodyJump = emitJump(compiler, OP_JUMP);
        int incrementStart = currentChunk(compiler)->count;
        expression(compiler);
        emitByte(compiler, OP_POP);
        consume(compiler, TK_RPAREN, "Expected ')' after for clauses.");

        emitLoop(compiler, loopStart);
        loopStart = incrementStart;
        patchJump(compiler, bodyJump);
    }

    statement(compiler);
    emitLoop(compiler, loopStart);

    if (exitJump != -1) {
        patchJump(compiler, exitJump);
        emitByte(compiler, OP_POP);
    }

    endScope(compiler);
}

static void
ifStatement(Compiler *compiler)
{
    consume(compiler, TK_LPAREN, "Expected '(' after 'if'.");
    expression(compiler);
    consume(compiler, TK_RPAREN, "Expected ')' after condition.");

    int thenJump = emitJump(compiler, OP_JUMP_FALSE);
    emitByte(compiler, OP_POP);
    statement(compiler);
    int elseJump = emitJump(compiler, OP_JUMP);
    patchJump(compiler, thenJump);
    emitByte(compiler, OP_POP);

    if (match(compiler, TK_ELSE)) statement(compiler);
    patchJump(compiler, elseJump);
}

static void
echoStatement(Compiler *compiler)
{
    expression(compiler);
    consume(compiler, TK_SEMICOLON, "Expected ';' after value.");
    emitByte(compiler, OP_ECHO);
}

static void
whileStatement(Compiler *compiler)
{
    int loopStart = currentChunk(compiler)->count;
    consume(compiler, TK_LPAREN, "Expected '(' after 'while'.");
    expression(compiler);
    consume(compiler, TK_RPAREN, "Expected ')' after condition.");

    int exitJump = emitJump(compiler, OP_JUMP_FALSE);
    emitByte(compiler, OP_POP);
    statement(compiler);
    emitLoop(compiler, loopStart);

    patchJump(compiler, exitJump);
    emitByte(compiler, OP_POP);
}

static void
synchronize(Parser *parser)
{
    parser->panicMode = false;

    while (parser->current.type != TK_EOF) {
        if (parser->previous.type == TK_SEMICOLON) return;
        switch (parser->current.type) {
            case TK_BREAK:
            case TK_CLASS:
            case TK_CONST:
            case TK_CONTINUE:
            case TK_ECHO:
            case TK_FN:
            case TK_FOR:
            case TK_IF:
            case TK_RETURN:
            case TK_VAR:
            case TK_WHILE:
                return;
            default:
                ; // Do nothing
        }

        advance(parser);
    }
}

static void
declaration(Compiler *compiler)
{
    if (match(compiler, TK_VAR)) {
        varDeclaration(compiler);
    } else {
        statement(compiler);
    }

    if (compiler->parser->panicMode) synchronize(compiler->parser);
}

static void
statement(Compiler *compiler)
{
    if (match(compiler, TK_ECHO)) {
        echoStatement(compiler);
    } else if (match(compiler, TK_FOR)) {
        forStatement(compiler);
    } else if (match(compiler, TK_IF)) {
        ifStatement(compiler);
    } else if (match(compiler, TK_WHILE)) {
        whileStatement(compiler);
    } else if (match(compiler, TK_LBRACE)) {
        beginScope(compiler);
        block(compiler);
        endScope(compiler);
    } else {
        expressionStatement(compiler);
    }
}

static void
_and(Compiler *compiler, bool canAssign)
{
    int endJump = emitJump(compiler, OP_JUMP_FALSE);
    emitByte(compiler, OP_POP);
    parsePrecedence(compiler, PREC_AND);
    patchJump(compiler, endJump);
}

static void
_or(Compiler *compiler, bool canAssign)
{
    int elseJump = emitJump(compiler, OP_JUMP_FALSE);
    int endJump = emitJump(compiler, OP_JUMP);

    patchJump(compiler, elseJump);
    emitByte(compiler, OP_POP);

    parsePrecedence(compiler, PREC_OR);
    patchJump(compiler, endJump);
}

ParseRule rules[] = {
    [TK_LPAREN]     = { grouping,   NULL,   PREC_NONE },
    [TK_RPAREN]     = { NULL,       NULL,   PREC_NONE },
    [TK_RBRACE]     = { NULL,       NULL,   PREC_NONE },
    [TK_LBRACE]     = { NULL,       NULL,   PREC_NONE },
    [TK_RBRACK]     = { NULL,       NULL,   PREC_NONE },
    [TK_LBRACK]     = { NULL,       NULL,   PREC_NONE },
    [TK_COMMA]      = { NULL,       NULL,   PREC_NONE },
    [TK_DOT]        = { NULL,       NULL,   PREC_NONE },
    [TK_QUESTION]   = { NULL,       NULL,   PREC_NONE },
    [TK_COLON]      = { NULL,       NULL,   PREC_NONE },
    [TK_SEMICOLON]  = { NULL,       NULL,   PREC_NONE },
    [TK_MINUS]      = { unary,      binary, PREC_TERM },
    [TK_PLUS]       = { NULL,       binary, PREC_TERM },
    [TK_SLASH]      = { NULL,       binary, PREC_FACTOR },
    [TK_STAR]       = { NULL,       binary, PREC_FACTOR },
    [TK_MODULUS]    = { NULL,       binary, PREC_FACTOR },
    [TK_POWER]      = { NULL,       binary, PREC_UNARY },
    [TK_BANG]       = { unary,      NULL,   PREC_NONE },
    [TK_BANGEQ]     = { NULL,       binary, PREC_EQUALITY },
    [TK_EQEQ]       = { NULL,       binary, PREC_EQUALITY },
    [TK_GREATER]    = { NULL,       binary, PREC_COMPARISON },
    [TK_GTEQ]       = { NULL,       binary, PREC_COMPARISON },
    [TK_LESS]       = { NULL,       binary, PREC_COMPARISON },
    [TK_LTEQ]       = { NULL,       binary, PREC_COMPARISON },
    [TK_BAND]       = { NULL,       binary, PREC_BITWISE },
    [TK_BOR]        = { NULL,       binary, PREC_BITWISE },
    [TK_BXOR]       = { NULL,       binary, PREC_BITWISE },
    [TK_SHL]        = { NULL,       binary, PREC_BITWISE },
    [TK_SHR]        = { NULL,       binary, PREC_BITWISE },
    [TK_INC]        = { NULL,       NULL,   PREC_NONE },
    [TK_DEC]        = { NULL,       NULL,   PREC_NONE },
    [TK_MINUSEQ]    = { NULL,       NULL,   PREC_NONE },
    [TK_PLUSEQ]     = { NULL,       NULL,   PREC_NONE },
    [TK_SLASHEQ]    = { NULL,       NULL,   PREC_NONE },
    [TK_STAREQ]     = { NULL,       NULL,   PREC_NONE },
    [TK_EQ]         = { NULL,       NULL,   PREC_NONE },
    [TK_IDENTIFIER] = { variable,   NULL,   PREC_NONE },
    [TK_STRING]     = { string,     NULL,   PREC_NONE },
    [TK_NUMBER]     = { number,     NULL,   PREC_NONE },
    [TK_AND]        = { NULL,       _and,   PREC_AND },
    [TK_BREAK]      = { NULL,       NULL,   PREC_NONE },
    [TK_CLASS]      = { NULL,       NULL,   PREC_NONE },
    [TK_CONST]      = { NULL,       NULL,   PREC_NONE },
    [TK_CONTINUE]   = { NULL,       NULL,   PREC_NONE },
    [TK_DO]         = { NULL,       NULL,   PREC_NONE },
    [TK_ELSE]       = { NULL,       NULL,   PREC_NONE },
    [TK_ECHO]       = { NULL,       NULL,   PREC_NONE },
    [TK_FALSE]      = { literal,    NULL,   PREC_NONE },
    [TK_FOR]        = { NULL,       NULL,   PREC_NONE },
    [TK_FN]         = { NULL,       NULL,   PREC_NONE },
    [TK_IF]         = { NULL,       NULL,   PREC_NONE },
    [TK_INCLUDE]    = { NULL,       NULL,   PREC_NONE },
    [TK_MODULE]     = { NULL,       NULL,   PREC_NONE },
    [TK_NULL]       = { literal,    NULL,   PREC_NONE },
    [TK_OR]         = { NULL,       _or,    PREC_OR },
    [TK_RETURN]     = { NULL,       NULL,   PREC_NONE },
    [TK_SELF]       = { NULL,       NULL,   PREC_NONE },
    [TK_SUPER]      = { NULL,       NULL,   PREC_NONE },
    [TK_TRUE]       = { literal,    NULL,   PREC_NONE },
    [TK_VAR]        = { NULL,       NULL,   PREC_NONE },
    [TK_WHILE]      = { NULL,       NULL,   PREC_NONE },
    [TK_ERROR]      = { NULL,       NULL,   PREC_NONE },
    [TK_EOF]        = { NULL,       NULL,   PREC_NONE },
};

static ParseRule *
getRule(TokenType type)
{
    return &rules[type];
}

static void
parsePrecedence(Compiler *compiler, Precedence precedence)
{
    advance(compiler->parser);
    ParseFn prefixRule = getRule(compiler->parser->previous.type)->prefix;
    if (prefixRule == NULL) {
        error(compiler->parser, "Expected expression.");
        return;
    }

    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(compiler, canAssign);

    while (precedence <= getRule(compiler->parser->current.type)->precedence) {
        advance(compiler->parser);
        ParseFn infixRule = getRule(compiler->parser->previous.type)->infix;
        infixRule(compiler, canAssign);
    }

    if (canAssign && match(compiler, TK_EQ)) {
        error(compiler->parser, "Invalid assignment target.");
    }
}

static uint8_t
identifierConstant(Compiler *compiler, Token *name)
{
    return makeConstant(
        compiler,
        OBJ_VAL(copyString(
            compiler->parser->vm, name->start, name->length)));
}

static bool
identifiersEqual(Token *a, Token *b)
{
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

static int
resolveLocal(Compiler *compiler, Token *name)
{
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];
        if (identifiersEqual(name, &local->name)) {
            if (local->depth == -1) {
                error(compiler->parser, "Can't read local variable in its own initializer.");
            }
            return i;
        }
    }

    return -1;
}

static void
addLocal(Compiler *compiler, Token name)
{
    if (compiler->localCount == UINT8_COUNT) {
        error(compiler->parser, "Too many local variables in function.");
        return;
    }

    Local *local = &compiler->locals[compiler->localCount++];
    local->name = name;
    local->depth = -1;
}

static void
declareVariable(Compiler *compiler)
{
    if (compiler->scopeDepth == 0) return;

    Token *name = &compiler->parser->previous;
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        Local *local = &compiler->locals[i];

        if (local->depth != -1 && local->depth < compiler->scopeDepth) {
            break;
        }

        if (identifiersEqual(name, &local->name)) {
            error(compiler->parser, "Already a variable with this name in this scope.");
        }
    }
    addLocal(compiler, *name);
}

static uint8_t
parseVariable(Compiler *compiler, const char *message)
{
    consume(compiler, TK_IDENTIFIER, message);

    declareVariable(compiler);
    if (compiler->scopeDepth > 0) return 0;

    return identifierConstant(compiler, &compiler->parser->previous);
}

static void
markInitialized(Compiler *compiler)
{
    if (compiler->scopeDepth == 0) return;
    compiler->locals[compiler->localCount - 1].depth = compiler->scopeDepth;
}

static void
defineVariable(Compiler *compiler, uint8_t global)
{
    if (compiler->scopeDepth > 0) {
        markInitialized(compiler);
        return;
    }

    emitBytes(compiler, OP_DEFINE_GLOBAL, global);
}

bool
compile(VM *vm, const char *src, Chunk *chunk)
{
    Parser parser;
    parser.vm = vm;
    parser.hadError = false;
    parser.panicMode = false;

    Lexer lexer;
    initLexer(&lexer, src);
    parser.lexer = lexer;

    Compiler compiler;
    initCompiler(&parser, &compiler, chunk);

    advance(compiler.parser);

    while (!match(&compiler, TK_EOF)) {
        declaration(&compiler);
    }

    endCompiler(&compiler);

    return !compiler.parser->hadError;
}
