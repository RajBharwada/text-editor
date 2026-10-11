/* edit.c - Person 2: Insertion & Editing
   Inserting a new line, replacing an existing line, and recording both for undo. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "core.h"
#include "edit.h"
#include "navigation.h"
#include "main.h"

/* Locate the position, then connect both forward and backward links. */
int insertLine(Document *document, int number, const char *text)
{
    Line *line;
    Line *next;
    if (document->count == INT_MAX || number < 1 || number - 1 > document->count)
        return 0;
    line = createLine(text);
    if (line == NULL)
        return 0;
    next = getLine(document, number);
    line->next = next;
    line->prev = next != NULL ? next->prev : document->tail;
    if (line->prev != NULL)
        line->prev->next = line;
    else
        document->head = line;
    if (next != NULL)
        next->prev = line;
    else
        document->tail = line;
    document->count++;
    return 1;
}

void insertLineCommand(Document *document, Action **top)
{
    int number;
    char text[256];
    if (promptNumber("Enter line position (1-based): ", &number) != 1)
        return;
    if (document->count == INT_MAX || number < 1 || number - 1 > document->count) {
        puts("Invalid line position.");
        return;
    }
    if (promptText("Enter text (up to 255 characters): ", text, sizeof text) != 1)
        return;
    /* Reserve undo memory before changing anything. */
    if (!pushUndo(top, ACTION_INSERT, number, "")) {
        puts("Out of memory. Document unchanged.");
        return;
    }
    if (!insertLine(document, number, text)) {
        free(popUndo(top));
        puts("Out of memory. Document unchanged.");
        return;
    }
    puts("Document updated.");
}

void editLineCommand(Document *document, Action **top)
{
    int number;
    char text[256];
    Line *line;
    if (promptNumber("Enter line position (1-based): ", &number) != 1)
        return;
    line = getLine(document, number);
    if (line == NULL) {
        puts("Invalid line position.");
        return;
    }
    if (promptText("Enter text (up to 255 characters): ", text, sizeof text) != 1)
        return;
    /* Reserve undo memory before changing anything. */
    if (!pushUndo(top, ACTION_EDIT, number, line->text)) {
        puts("Out of memory. Document unchanged.");
        return;
    }
    strcpy(line->text, text);
    puts("Document updated.");
}
