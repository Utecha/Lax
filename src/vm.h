#ifndef LAX_VM_H
#define LAX_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

/*
 * Structure keeping track of the data being
 * managed and interpreted by the VM.
 * It takes chunks of bytecode and interprets it.
*/
typedef struct {
    Chunk *chunk;
    uint8_t *ip;

    Value stack[STACK_MAX];
    Value *stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

/*
 * Initializes the data for the VM.
*/
void
initVM(VM *vm);

/*
 * Free's all memory allocated for the VM.
*/
void
freeVM(VM *vm);

/*
 * Sets everything up for the VM to interpret the 
 * compiled Bytecode. Passes the actual logic of
 * interpretation onto a static 'run' function
 * in the implementation file.
*/
InterpretResult
interpret(VM *vm, const char *src);

void
push(VM *vm, Value value);

Value
pop(VM *vm);

#endif // LAX_VM_H
