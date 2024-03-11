#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

// Very simple REPL for interpreting code.
// Not super useful for testing functions or classes in its current state.
static void
repl()
{
    char line[1024];
    for (;;) {
        printf(">>> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        if (!strncmp(line, "exit", 4)) {
            break;
        }
    }
}

/* Start her up! */
int
main(int argc, char **argv)
{
    VM vm;
    initVM(&vm);
    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 4);
    appendChunk(&chunk, OP_CONSTANT, 69);
    appendChunk(&chunk, constant, 69);

    constant = addConstant(&chunk, 4);
    appendChunk(&chunk, OP_CONSTANT, 69);
    appendChunk(&chunk, constant, 69);

    appendChunk(&chunk, OP_POWER, 69);

    constant = addConstant(&chunk, 23);
    appendChunk(&chunk, OP_CONSTANT, 69);
    appendChunk(&chunk, constant, 69);

    constant = addConstant(&chunk, 5);
    appendChunk(&chunk, OP_CONSTANT, 69);
    appendChunk(&chunk, constant, 69);

    appendChunk(&chunk, OP_MODULO, 69);
    appendChunk(&chunk, OP_ADD, 69);
    appendChunk(&chunk, OP_NEGATE, 69);
    appendChunk(&chunk, OP_RETURN, 69);

    disassembleChunk(&chunk, "Test Chunk");
    interpret(&vm, &chunk);

    freeVM(&vm);
    freeChunk(&chunk);

    return 0;
}
