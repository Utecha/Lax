#include "chunk.h"
#include "memory.h"

void
initChunk(Chunk *chunk)
{
    chunk->code = NULL;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
    chunk->count = 0;
    chunk->capacity = 0;
}

void
freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void
appendChunk(Chunk *chunk, uint8_t byte, int line)
{
    if (chunk->count + 1 > chunk->capacity) {
        int oldCap = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCap);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCap, chunk->capacity);
        chunk->lines = GROW_ARRAY(int, chunk->lines, oldCap, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

int
addConstant(Chunk *chunk, Value value)
{
    appendValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}
