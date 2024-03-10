#include <stdio.h>

#include "memory.h"
#include "value.h"

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
    if (array->count + 1 > array->capacity) {
        int oldCap = array->capacity;
        array->capacity = GROW_CAPACITY(oldCap);
        array->values = GROW_ARRAY(Value, array->values, oldCap, array->capacity);
    }

    array->values[array->count++] = value;
}

void
printValue(Value value)
{
    printf("%g", value);
}
