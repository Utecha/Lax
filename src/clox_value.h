#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include "clox_common.h"

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_NUMBER,
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

#define IS_BOOL(value)      ((value).type == VAL_BOOL)
#define IS_NIL(value)       ((value).type == VAL_NIL)
#define IS_NUMBER(value)    ((value).type == VAL_NUMBER)

#define AS_BOOL(value)      ((value).as.boolean)
#define AS_NUMBER(value)    ((value).as.number)

#define BOOL_VAL(value)     ((Value){ VAL_BOOL, {.boolean = value} })
#define NIL_VAL             ((Value){ VAL_NIL, {.number = 0} })
#define NUMBER_VAL(value)   ((Value){ VAL_NUMBER, {.number = value} })

#define UINT8_COUNT     (UINT8_MAX + 1)

typedef struct {
    Value *values;
    int count;
    int capacity;
} ValueArray;

bool valuesEqual(Value a, Value b);
void freeValueArray(ValueArray *array);
void initValueArray(ValueArray *array);
void printValue(Value value);
void writeValueArray(ValueArray *array, Value value);

#endif // CLOX_VALUE_H
