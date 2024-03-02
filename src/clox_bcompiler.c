#include <stdio.h>
#include <stdlib.h>

#include "clox_bcompiler.h"
#include "clox_common.h"

#ifdef DEBUG_PRINT_CODE
#include "clox_debug.h"
#endif

Parser parser;
Chunk *compilingChunk;

static Chunk *currentChunk()
{
    return compilingChunk;
}

static void errorAt(Token *token, const char *message)
{
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);

    parser.hadError = true;
}

static void error(const char *message)
{
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}

static void advance()
{
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void endCompiler()
{
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "Byte Code");
    }
#endif // DEBUG_PRINT_CODE
}

static void parsePrecedence(Precedence precedence);
static ParseRule *getRule(TokenType type);
static void expression();

static void binary()
{
    TokenType opType = parser.previous.type;
    ParseRule *rule = getRule(opType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (opType) {
        case TOKEN_NOTEQ:   emitBytes(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQEQ:    emitByte(OP_EQUAL); break;
        case TOKEN_GT:      emitByte(OP_GREATER); break;
        case TOKEN_GTEQ:    emitBytes(OP_LESS, OP_NOT); break;
        case TOKEN_LT:      emitByte(OP_LESS); break;
        case TOKEN_LTEQ:    emitBytes(OP_GREATER, OP_NOT); break;
        case TOKEN_PLUS:    emitByte(OP_ADD); break;
        case TOKEN_MINUS:   emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:    emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:   emitByte(OP_DIVIDE); break;
        default:            return; // Unreachable
    }
}

static void literal()
{
    switch (parser.previous.type) {
        case TOKEN_FALSE:       emitByte(OP_FALSE); break;
        case TOKEN_NIL:         emitByte(OP_NIL); break;
        case TOKEN_TRUE:        emitByte(OP_TRUE); break;
        default:                return; // Unreachable
    }
}

static void grouping()
{
    expression();
    consume(TOKEN_RPAREN, "Expected ')' after expression.");
}

static void number()
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

static void unary()
{
    TokenType opType = parser.previous.type;

    // Compile the operand
    parsePrecedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (opType) {
        case TOKEN_MINUS:   emitByte(OP_NEGATE); break;
        case TOKEN_NOT:     emitByte(OP_NOT); break;
        default:            return; // Unreachable
    }
}

ParseRule rules[] = {
    [TOKEN_LPAREN]     = { grouping,   NULL,    PREC_NONE },
    [TOKEN_RPAREN]     = { NULL,       NULL,    PREC_NONE },
    [TOKEN_LBRACE]     = { NULL,       NULL,    PREC_NONE },
    [TOKEN_RBRACE]     = { NULL,       NULL,    PREC_NONE },
    [TOKEN_COMMA]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_DOT]        = { NULL,       NULL,    PREC_NONE },
    [TOKEN_MINUS]      = { unary,      binary,  PREC_TERM },
    [TOKEN_PLUS]       = { NULL,       binary,  PREC_TERM },
    [TOKEN_SEMICOLON]  = { NULL,       NULL,    PREC_NONE },
    [TOKEN_SLASH]      = { NULL,       binary,  PREC_FACTOR },
    [TOKEN_STAR]       = { NULL,       binary,  PREC_FACTOR },
    [TOKEN_NOT]        = { unary,      NULL,    PREC_NONE },
    [TOKEN_NOTEQ]      = { NULL,       binary,  PREC_EQUALITY },
    [TOKEN_EQ]         = { NULL,       NULL,    PREC_NONE },
    [TOKEN_EQEQ]       = { NULL,       binary,  PREC_EQUALITY },
    [TOKEN_GT]         = { NULL,       binary,  PREC_COMPARISON },
    [TOKEN_GTEQ]       = { NULL,       binary,  PREC_COMPARISON },
    [TOKEN_LT]         = { NULL,       binary,  PREC_COMPARISON },
    [TOKEN_LTEQ]       = { NULL,       binary,  PREC_COMPARISON },
    [TOKEN_IDENTIFIER] = { NULL,       NULL,    PREC_NONE },
    [TOKEN_STRING]     = { NULL,       NULL,    PREC_NONE },
    [TOKEN_NUMBER]     = { number,     NULL,    PREC_NONE },
    [TOKEN_AND]        = { NULL,       NULL,    PREC_NONE },
    [TOKEN_CLASS]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_ELSE]       = { NULL,       NULL,    PREC_NONE },
    [TOKEN_FALSE]      = { literal,    NULL,    PREC_NONE },
    [TOKEN_FOR]        = { NULL,       NULL,    PREC_NONE },
    [TOKEN_FUN]        = { NULL,       NULL,    PREC_NONE },
    [TOKEN_IF]         = { NULL,       NULL,    PREC_NONE },
    [TOKEN_NIL]        = { literal,    NULL,    PREC_NONE },
    [TOKEN_OR]         = { NULL,       NULL,    PREC_NONE },
    [TOKEN_PRINT]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_RETURN]     = { NULL,       NULL,    PREC_NONE },
    [TOKEN_SELF]       = { NULL,       NULL,    PREC_NONE },
    [TOKEN_SUPER]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_TRUE]       = { literal,    NULL,    PREC_NONE },
    [TOKEN_VAR]        = { NULL,       NULL,    PREC_NONE },
    [TOKEN_WHILE]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_ERROR]      = { NULL,       NULL,    PREC_NONE },
    [TOKEN_EOF]        = { NULL,       NULL,    PREC_NONE },
};

static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expected expression.");
        return;
    }
    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule *getRule(TokenType type)
{
    return &rules[type];
}

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

bool bCompile(Chunk *chunk, const char *source)
{
    // Initialize the Scanner
    initScanner(source);

    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    // Advance the parser through the source
    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression.");

    // End compilation
    endCompiler();

    return !parser.hadError;
}
