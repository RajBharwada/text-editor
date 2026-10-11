/* core.c - Person 1: Core Text/Data Structure
   Creates line nodes, frees the document, and owns the undo stack. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "edit.h"
#include "delete.h"
#include "navigation.h"

Line *createLine(const char *text)
{
    Line *line = malloc(sizeof *line);
    if (line != NULL) {
        strcpy(line->text, text);
        line->prev = NULL;
        line->next = NULL;
    }
    return line;
}

/* The top of a linked-list stack is its head: push and pop take O(1). */
int pushUndo(Action **top, ActionType type, int number, const char *text)
{
    Action *action = malloc(sizeof *action);
    if (action == NULL)
        return 0;
    action->type = type;
    action->lineNumber = number;
    strcpy(action->text, text);
    action->next = *top;
    *top = action;
    return 1;
}

Action *popUndo(Action **top)
{
    Action *action = *top;
    if (action != NULL)
        *top = action->next;
    return action;
}

void freeUndoStack(Action **top)
{
    Action *action;
    while ((action = popUndo(top)) != NULL)
        free(action);
}

void freeDocument(Document *document)
{
    Line *line = document->head;
    while (line != NULL) {
        Line *next = line->next;
        free(line);
        line = next;
    }
    document->head = NULL;
    document->tail = NULL;
    document->count = 0;
}

void undo(Document *document, Action **top)
{
    Action *action = *top;
    Line *line;
    if (action == NULL) {
        puts("Nothing to undo.");
        return;
    }
    /* Use list operations directly so undo never pushes another action. */
    if (action->type == ACTION_DELETE) {
        if (!insertLine(document, action->lineNumber, action->text)) {
            puts("Could not restore line. Undo remains available.");
            return;
        }
    } else {
        line = getLine(document, action->lineNumber);
        if (action->type == ACTION_INSERT)
            deleteLine(document, line);
        else
            strcpy(line->text, action->text);
    }
    free(popUndo(top));
    puts("Undo completed.");
}
