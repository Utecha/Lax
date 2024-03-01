#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "clox_common.h"

typedef double Value;

#define UINT8_COUNT     (UINT8_MAX + 1)

typedef struct {
    Value *values;
    int count;
    int capacity;
} ValueArray;

void freeValueArray(ValueArray *array);
void initValueArray(ValueArray *array);
void printValue(Value value);
void writeValueArray(ValueArray *array, Value value);

#endif // CLOX_VALUE_H
