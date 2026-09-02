#ifndef CORE_H
#define CORE_H

#include <stddef.h>

#define INITIAL_LINE_CAPACITY 64

typedef struct LineNode {
    char *text;
    size_t length;
    size_t capacity;

    struct LineNode *prev;
    struct LineNode *next;
} LineNode;

int EnsureLineCapacity(LineNode *line, size_t requiredCapacity);

#endif