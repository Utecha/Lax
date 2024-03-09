#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include "clox_chunk.h"

int disassembleInstruction(Chunk *chunk, int offset);
void disassembleChunk(Chunk *chunk, const char *name);

#endif // CLOX_DEBUG_H
