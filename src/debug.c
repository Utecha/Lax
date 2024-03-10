#include "debug.h"
#include "log.h"
#include "value.h"

void
disassembleChunk(Chunk *chunk, const char *name)
{
    printf("     === %s ===\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

// Single Byte Instructions
static int
simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

// Two Byte Instructions
static int
constantInstruction(const char *name, Chunk *chunk, int offset)
{
    uint8_t constant = chunk->code[offset + 1];

    // Instruction Name     Constant Index      'Constant Value'
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");

    return offset + 2;
}

int
disassembleInstruction(Chunk *chunk, int offset)
{
    // The byte offset
    printf("%04d ", offset);

    // The line in which the instruction is located
    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[offset]);
    }

    // The byte instruction
    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        default: {
            laxlog(ERROR, "Unknown OpCode: %d\n", instruction);
            return offset + 1;
        }
        case OP_CONSTANT:   return constantInstruction("OP_CONSTANT", chunk, offset);
        case OP_RETURN:     return simpleInstruction("OP_RETURN", offset);
    }
}
