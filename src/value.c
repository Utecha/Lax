#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"

bool
valuesEqual(Value a, Value b)
{
    if (a.type != b.type)   return false;
    switch (a.type) {
        case VAL_BOOL:      return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NULL:      return true;
        case VAL_NUMBER:    return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:       return AS_OBJ(a) == AS_OBJ(b);
        default:            return false; // Unreachable
    }
}

void
initValueArray(ValueArray *array)
{
    array->values = NULL;
    array->count = 0;
    array->capacity = 0;
}

void
freeValueArray(ValueArray *array)
{
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void
appendValueArray(ValueArray *array, Value value)
{
    if (array->capacity < array->count + 1) {
        int oldCap = array->capacity;
        array->capacity = GROW_CAPACITY(oldCap);
        array->values = GROW_ARRAY(Value, array->values, oldCap, array->capacity);
    }

    array->values[array->count++] = value;
}

void
printValue(Value value)
{
    switch (value.type) {
        case VAL_BOOL: {
            printf(AS_BOOL(value) ? "true" : "false");
        } break;
        case VAL_NULL:      printf("null");                 break;
        case VAL_NUMBER:    printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ:       printObject(value);             break;
    }
}
