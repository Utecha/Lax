#ifndef LAX_CHUNK_H
#define LAX_CHUNK_H

#include "common.h"
#include "value.h"

/*
 * Enum of all of the OpCodes used to emit the 
 * correct bytes for compilation.
*/
typedef enum {
    OP_CONSTANT,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULUS,
    OP_POWER,
    OP_NOT,
    OP_BAND,
    OP_BOR,
    OP_BXOR,
    OP_SHL,
    OP_SHR,
    OP_NEGATE,
    OP_ECHO,
    OP_RETURN
} OpCode;

/*
 * 8-bit Dynamic Array (Array of Bytes)
*/
typedef struct {
    uint8_t *code;
    ValueArray constants;
    int *lines;
    int count;
    int capacity;
} Chunk;

/*
 * Initializes a Bytecode Chunk.
*/
void
initChunk(Chunk *chunk);

/*
 * Free's memory allocated by a Bytecode Chunk.
*/
void
freeChunk(Chunk *chunk);

/*
 * Appends a byte instruction to a Bytecode Chunk.
*/
void
appendChunk(Chunk *chunk, uint8_t byte, int line);

/*
 * Add's a constant to the constants table
*/
int
addConstant(Chunk *chunk, Value value);

#endif // CHUNK_H
