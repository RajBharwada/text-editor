/* delete.c - Person 3: Deletion Operations
   Removing a line (including the head/tail edge cases) and recording it for undo. */
#include <stdio.h>
#include <stdlib.h>
#include "core.h"
#include "delete.h"
#include "navigation.h"
#include "main.h"

/* Reconnect the neighbors before freeing the removed node. */
void deleteLine(Document *document, Line *line)
{
    if (line->prev != NULL)
        line->prev->next = line->next;
    else
        document->head = line->next;
    if (line->next != NULL)
        line->next->prev = line->prev;
    else
        document->tail = line->prev;
    free(line);
    document->count--;
}

void deleteLineCommand(Document *document, Action **top)
{
    int number;
    Line *line;
    if (promptNumber("Enter line position (1-based): ", &number) != 1)
        return;
    line = getLine(document, number);
    if (line == NULL) {
        puts("Invalid line position.");
        return;
    }
    /* Reserve undo memory before changing anything. */
    if (!pushUndo(top, ACTION_DELETE, number, line->text)) {
        puts("Out of memory. Document unchanged.");
        return;
    }
    deleteLine(document, line);
    puts("Document updated.");
}
