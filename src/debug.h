#ifndef LAX_DEBUG_H
#define LAX_DEBUG_H

#include "chunk.h"

/*
 * Disassembles an entire Bytecode Chunk.
*/
void
disassembleChunk(Chunk *chunk, const char *name);

/*
 * Disassemble a specific byte instruction
 *
 * Returns the byte offset of the next instruction.
*/
int
disassembleInstruction(Chunk *chunk, int offset);

#endif // LAX_DEBUG_H
