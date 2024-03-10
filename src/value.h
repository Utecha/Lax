#ifndef LAX_VALUE_H
#define LAX_VALUE_H

#include "common.h"

/*
 * Generic 'Value' type used to introduce
 * dynamic typing.
*/
typedef double Value;

/*
 * Dynamic Array of 8-bit 'Value' values
*/
typedef struct {
    Value *values;
    int count;
    int capacity;
} ValueArray;

/*
 * Initializes an 8-bit ValueArray.
*/
void
initValueArray(ValueArray *array);

/*
 * Free's memory allocated by an 8-bit
 * ValueArray.
*/
void
freeValueArray(ValueArray *array);

/*
 * Appends a 'Value' to an 8-bit ValueArray.
*/
void
appendValueArray(ValueArray *array, Value value);

/*
 * Helper function for printing 'Value' constants.
*/
void
printValue(Value value);

#endif // LAX_VALUE_H
