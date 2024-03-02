#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include "clox_common.h"
#include "clox_value.h"

typedef enum {
    OP_CONSTANT,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct {
    uint8_t *code;
    ValueArray constants;
    int *lines;
    int count;
    int capacity;
} Chunk;

int addConstant(Chunk *chunk, Value value);
void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);

#endif // CLOX_CHUNK_H
