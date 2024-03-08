#ifndef CLOX_VM_H
#define CLOX_VM_H

#include "clox_chunk.h"
#include "clox_object.h"
#include "clox_table.h"
#include "clox_value.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    ObjClosure *closure;
    uint8_t *ip;
    Value *slots;
} CallFrame;

typedef struct {
    // Call Frames
    CallFrame frames[FRAMES_MAX];
    int frameCount;

    // VM stack
    Value stack[STACK_MAX];
    Value *stackTop;
    Table globals;
    Table strings;
    ObjUpvalue *openUpvalues;

    // Manage GC timing
    size_t bytesAllocated;
    size_t nextGC;
    Obj *objects;

    // GC Markers
    int grayCount;
    int grayCapacity;
    Obj **grayStack;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char *source);
void push(Value value);
Value pop();

#endif // CLOX_VM_H
