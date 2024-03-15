#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "clox_common.h"
#include "clox_object.h"

#define ALLOCATE(type, count)                               \
    (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE_ARRAY(type, pointer, oldCount)                 \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

#define FREE(type, pointer)                                 \
    reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity)                             \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount)       \
    (type *)reallocate(pointer, sizeof(type) * (oldCount),  \
        sizeof(type) * (newCount))

void *reallocate(void *pointer, size_t oldSize, size_t newSize);
void markObject(Obj *object);
void markValue(Value value);
void collectGarbage();
void freeObjects();

#endif // CLOX_MEMORY_H