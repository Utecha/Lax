#ifndef LAX_OBJECT_H
#define LAX_OBJECT_H

#include "chunk.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define OBJ_TYPE(value)         (AS_OBJ(value)->type)
#define IS_STRING(value)        isObjType(value, OBJ_STRING)
#define AS_STRING(value)        ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value)       (((ObjString *)AS_OBJ(value))->chars)

typedef enum {
    OBJ_STRING,
} ObjType;

struct Obj {
    ObjType type;
    struct Obj *next;
};

struct ObjString {
    struct Obj obj;
    int length;
    char *chars;
    uint32_t hash;
};

ObjString *
takeString(VM *vm, char *chars, int length);

ObjString *
copyString(VM *vm, const char *chars, int length);

void
printObject(Value value);

static inline bool
isObjType(Value value, ObjType type)
{
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif // LAX_OBJECT_H
