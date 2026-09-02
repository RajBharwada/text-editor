#include "core.h"
#include <stdlib.h>

LineNode *CreateLineNode(void)
{
    LineNode *node = malloc(sizeof(LineNode));

    if (node == NULL)
        return NULL;

    node->capacity = INITIAL_LINE_CAPACITY;
    node->length = 0;

    node->text = malloc(node->capacity * sizeof(char));

    if (node->text == NULL) {
        free(node);
        return NULL;
    }

    node->text[0] = '\0';

    node->prev = NULL;
    node->next = NULL;

    return node;
}