#include "bcompiler.h"
#include "chunk.h"
#include "object.h"
#include "value.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif // DEBUG_PRINT_CODE

// Chunk *compilingChunk;

// Begin Parser

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

// Begin Compiler

static Chunk *
currentChunk(Compiler *compiler)
{
    return compiler->compiling;
}

static void
emitByte(Compiler *compiler, uint8_t byte)
{
    appendChunk(currentChunk(compiler), byte, compiler->parser->previous.line);
}

static void
emitBytes(Compiler *compiler, uint8_t byte, uint8_t byte2)
{
    emitByte(compiler, byte);
    emitByte(compiler, byte2);
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
initCompiler(Parser *parser, Compiler *compiler, Chunk *chunk)
{
    compiler->parser = parser;
    compiler->compiling = chunk;
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
// End Compiler

static void
expression(Compiler *compiler);

static ParseRule *
getRule(TokenType type);

static void
parsePrecedence(Compiler *compiler, Precedence precedence);

static void
binary(Compiler *compiler)
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
literal(Compiler *compiler)
{
    switch (compiler->parser->previous.type) {
        case TK_FALSE:      emitByte(compiler, OP_FALSE); break;
        case TK_NULL:       emitByte(compiler, OP_NULL);  break;
        case TK_TRUE:       emitByte(compiler, OP_TRUE);  break;
        default:            return; // Unreachable
    }
}

static void
grouping(Compiler *compiler)
{
    expression(compiler);
    consume(compiler, TK_RPAREN, "Expected ')' after expression.");
}

static void
expression(Compiler *compiler)
{
    parsePrecedence(compiler, PREC_ASSIGNMENT);
}

static void
number(Compiler *compiler)
{
    double value = strtod(compiler->parser->previous.start, NULL);
    emitConstant(compiler, NUMBER_VAL(value));
}

static void
string(Compiler *compiler)
{
    emitConstant(
        compiler,
        OBJ_VAL(copyString(
                compiler->parser->vm,
                compiler->parser->previous.start + 1,
                compiler->parser->previous.length - 2)
        )
    );
}

static void
unary(Compiler *compiler)
{
    TokenType opType = compiler->parser->previous.type;
    parsePrecedence(compiler, PREC_UNARY);

    switch (opType) {
        case TK_MINUS:  emitByte(compiler, OP_NEGATE);  break;
        case TK_BANG:   emitByte(compiler, OP_NOT);     break;
        default:        return; // Unreachable
    }
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
    [TK_IDENTIFIER] = { NULL,       NULL,   PREC_NONE },
    [TK_STRING]     = { string,     NULL,   PREC_NONE },
    [TK_NUMBER]     = { number,     NULL,   PREC_NONE },
    [TK_AND]        = { NULL,       NULL,   PREC_NONE },
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
    [TK_MODULE]     = { NULL,       NULL,   PREC_NONE },
    [TK_NULL]       = { literal,    NULL,   PREC_NONE },
    [TK_OR]         = { NULL,       NULL,   PREC_NONE },
    [TK_RETURN]     = { NULL,       NULL,   PREC_NONE },
    [TK_SELF]       = { NULL,       NULL,   PREC_NONE },
    [TK_SUPER]      = { NULL,       NULL,   PREC_NONE },
    [TK_TRUE]       = { literal,    NULL,   PREC_NONE },
    [TK_USING]      = { NULL,       NULL,   PREC_NONE },
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
    prefixRule(compiler);

    while (precedence <= getRule(compiler->parser->current.type)->precedence) {
        advance(compiler->parser);
        ParseFn infixRule = getRule(compiler->parser->previous.type)->infix;
        infixRule(compiler);
    }
}
// End Parser

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
    expression(&compiler);

    consume(&compiler, TK_EOF, "Expected end of expression.");
    endCompiler(&compiler);

    return !compiler.parser->hadError;
}
