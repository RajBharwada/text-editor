/* navigation.c - Person 4: Navigation & Search
   Moving through the list by line number and printing it.
   (Cursor/column tracking and text search are not in the original program.) */
#include <stdio.h>
#include "core.h"
#include "navigation.h"

Line *getLine(const Document *document, int number)
{
    Line *line = document->head;
    if (number < 1 || number > document->count)
        return NULL;
    while (--number > 0)
        line = line->next;
    return line;
}

void displayDocument(const Document *document)
{
    Line *line = document->head;
    int number = 1;
    if (line == NULL)
        puts("Document is empty.");
    while (line != NULL) {
        printf("%d | %s\n", number++, line->text);
        line = line->next;
    }
}
