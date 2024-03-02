#ifndef CLOX_TABLE_H
#define CLOX_TABLE_H

#include "clox_common.h"
#include "clox_value.h"

typedef struct {
    ObjString *key;
    Value value;
} Entry;

typedef struct {
    Entry *entries;
    int count;
    int capacity;
} Table;

void initTable(Table *table);
void freeTable(Table *table);
bool tableGet(Table *table, ObjString *key, Value *value);
bool tableSet(Table *table, ObjString *key, Value value);
bool tableDelete(Table *table, ObjString *key);
void tableAddAll(Table *from, Table *to);
ObjString *tableFindString(Table *table, const char *chars, int length, uint32_t hash);

#endif // CLOX_TABLE_H
