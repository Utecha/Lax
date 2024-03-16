#ifndef LAX_BCOMPILER_H
#define LAX_BCOMPILER_H

#include "common.h"
#include "lexer.h"
#include "vm.h"

typedef struct {
    VM *vm;
    Lexer lexer;
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_CONDITIONAL,
    PREC_BITWISE,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef struct {
    Token name;
    int depth;
} Local;

typedef struct Compiler {
    // Parser & currently compiling Chunk
    Parser *parser;
    Chunk *compiling;

    // Data for local variables
    Local locals[UINT8_COUNT];
    int localCount;

    int scopeDepth; // Depth of variable scope. 0 = global
} Compiler;

typedef void (*ParseFn)(Compiler *compiler, bool canAssign);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

/*
 * Initializes the Lexer, Parser, and Compiler,
 * and compiles the input source code to
 * Lax Bytecode.
*/
bool
compile(VM *vm, const char *src, Chunk *chunk);

#endif // LAX_BCOMPILER_H
