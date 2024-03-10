#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"

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
    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 2);
    appendChunk(&chunk, OP_CONSTANT, 69);
    appendChunk(&chunk, constant, 69);

    appendChunk(&chunk, OP_RETURN, 69);

    disassembleChunk(&chunk, "Test Chunk");

    freeChunk(&chunk);

    return 0;
}
