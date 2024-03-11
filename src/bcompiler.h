#ifndef LAX_BCOMPILER_H
#define LAX_BCOMPILER_H

#include "vm.h"

/*
 * Initializes the Lexer, Parser, and Compiler,
 * and compiles the input source code to
 * Lax Bytecode.
*/
void
compile(VM *vm, const char *src);

#endif // LAX_BCOMPILER_H
