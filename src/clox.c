#include "clox_chunk.h"
#include "clox_common.h"
#include "clox_debug.h"
#include "clox_vm.h"

int main(int argc, char **argv)
{
    // Initialize the VM
    VM vm;
    initVM(&vm);

    Chunk chunk;
    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_ADD, 123);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, constant, 123);

    writeChunk(&chunk, OP_DIVIDE, 123);
    writeChunk(&chunk, OP_NEGATE, 123);
    writeChunk(&chunk, OP_RETURN, 123);

    // disassembleChunk(&chunk, "Test Chunk");
    interpret(&vm, &chunk);

    // Free memory allocated by the VM
    freeVM(&vm);
    freeChunk(&chunk);

    return 0;
}
