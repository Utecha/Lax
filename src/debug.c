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
        case OP_NULL:       return simpleInstruction("OP_NULL", offset);
        case OP_TRUE:       return simpleInstruction("OP_TRUE", offset);
        case OP_FALSE:      return simpleInstruction("OP_FALSE", offset);
        case OP_EQUAL:      return simpleInstruction("OP_EQUAL", offset);
        case OP_GREATER:    return simpleInstruction("OP_GREATER", offset);
        case OP_LESS:       return simpleInstruction("OP_LESS", offset);
        case OP_ADD:        return simpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:   return simpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:   return simpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:     return simpleInstruction("OP_DIVIDE", offset);
        case OP_MODULUS:    return simpleInstruction("OP_MODULO", offset);
        case OP_POWER:      return simpleInstruction("OP_POWER", offset);
        case OP_NOT:        return simpleInstruction("OP_NOT", offset);
        case OP_BAND:       return simpleInstruction("OP_BAND", offset);
        case OP_BOR:        return simpleInstruction("OP_BOR", offset);
        case OP_BXOR:       return simpleInstruction("OP_BXOR", offset);
        case OP_SHL:        return simpleInstruction("OP_SHL", offset);
        case OP_SHR:        return simpleInstruction("OP_SHR", offset);
        case OP_NEGATE:     return simpleInstruction("OP_NEGATE", offset);
        case OP_RETURN:     return simpleInstruction("OP_RETURN", offset);
    }
}
