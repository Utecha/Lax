#ifndef CLOX_OBJECT_H
#define CLOX_OBJECT_H

#include "clox_common.h"
#include "clox_chunk.h"
#include "clox_table.h"
#include "clox_value.h"

// Checks if the value is of type Obj
#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

// Checks more specifically what type of Obj it is
#define IS_BOUND_METHOD(value)  isObjType(value, OBJ_BOUND_METHOD)
#define IS_CLASS(value)         isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value)       isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)      isObjType(value, OBJ_FUNCTION)
#define IS_INSTANCE(value)      isObjType(value, OBJ_INSTANCE)
#define IS_NATIVE(value)        isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)        isObjType(value, OBJ_STRING)

// Casts the value to that Obj type
#define AS_BOUND_METHOD(value)  ((ObjBoundMethod *)AS_OBJ(value))
#define AS_CLASS(value)         ((ObjClass *)AS_OBJ(value))
#define AS_CLOSURE(value)       ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value)      ((ObjFunction *)AS_OBJ(value))
#define AS_INSTANCE(value)      ((ObjInstance *)AS_OBJ(value))
#define AS_NATIVE(value)        (((ObjNative *)AS_OBJ(value))->function)
#define AS_STRING(value)        ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value)       (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
} ObjType;

struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj *next;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString *name;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value *args, ...);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

struct ObjString {
    Obj obj;
    int length;
    char *chars;
    uint32_t hash;
};

typedef struct ObjUpvalue {
    Obj obj;
    Value *location;
    Value closed;
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalueCount;
} ObjClosure;

typedef struct {
    Obj obj;
    ObjString *name;
    Table methods;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass *class;
    Table fields;
} ObjInstance;

typedef struct {
    Obj obj;
    Value receiver;
    ObjClosure *method;
} ObjBoundMethod;

ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method);
ObjClass *newClass(ObjString *name);
ObjUpvalue *newUpvalue(Value *slot);
ObjClosure *newClosure(ObjFunction *function);
ObjFunction *newFunction();
ObjInstance *newInstance(ObjClass *class);
ObjNative *newNative(NativeFn function);
ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif // CLOX_OBJECT_H
