#ifndef LAX_VALUE_H
#define LAX_VALUE_H

#include "common.h"

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
    VAL_OBJ,
    // VAL_INTEGER,
    // VAL_DOUBLE,
} ValueType;

/*
 * Generic 'Value' type used to introduce
 * dynamic typing.
*/
typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } as;
} Value;

/*
 * Returns 'true' if the 'Value' type tag matches.
*/
#define IS_BOOL(value)          ((value).type == VAL_BOOL)
#define IS_NULL(value)          ((value).type == VAL_NULL)
#define IS_NUMBER(value)        ((value).type == VAL_NUMBER)
#define IS_OBJ(value)           ((value).type == VAL_OBJ)

/*
 * Move the in opposite direction of the {TYPE}_VAL macros below.
 * This unpacks the value to produce the raw C value for us to work
 * with in the VM.
*/
#define AS_BOOL(value)          ((value).as.boolean)
#define AS_NUMBER(value)        ((value).as.number)
#define AS_OBJ(value)           ((value).as.obj)

/*
 * Takes a C value of the appropriate type and produces a 'Value' type
 * with the correct type tag and the underlying value.
*/
#define BOOL_VAL(value)         ((Value){VAL_BOOL, {.boolean = value}})
#define NULL_VAL                ((Value){VAL_NULL, {.number = 0}})
#define NUMBER_VAL(value)       ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)         ((Value){VAL_OBJ, {.obj = (Obj *)object}})

/*
 * Dynamic Array of 8-bit 'Value' values
*/
typedef struct {
    Value *values;
    int count;
    int capacity;
} ValueArray;

bool
valuesEqual(Value a, Value b);
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
