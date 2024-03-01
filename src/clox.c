#include "clox_chunk.h"
#include "clox_common.h"
#include "clox_debug.h"

int main(int argc, char **argv)
{
    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_RETURN, 123);

    disassembleChunk(&chunk, "Test Chunk");
    freeChunk(&chunk);
    return 0;
}
