#ifndef LAX_TABLE_H
#define LAX_TABLE_H

#include "common.h"
#include "value.h"

typedef struct {
    ObjString *key;
    Value value;
} Entry;

typedef struct {
    Entry *entries;
    int count;
    int capacity;
} Table;

void
initTable(Table *table);

void
freeTable(Table *table);

bool
tableSet(Table *table, ObjString *key, Value value);

bool
tableGet(Table *table, ObjString *key, Value *value);

bool
tableDelete(Table *table, ObjString *key);

void
tableAddAll(Table *from, Table *to);

ObjString *
tableFindString(Table *table, const char *chars, int length, uint32_t hash);

#endif // LAX_TABLE_H
