/* core.h - Person 1: Core Text/Data Structure
   Document representation (doubly linked list of lines) and the undo stack
   that records changes to it. */
#ifndef CORE_H
#define CORE_H

typedef struct Line {
    char text[256];
    struct Line *prev;
    struct Line *next;
} Line;

typedef struct Document {
    Line *head;
    Line *tail;
    int count;
} Document;

typedef enum { ACTION_INSERT, ACTION_DELETE, ACTION_EDIT } ActionType;

typedef struct Action {
    ActionType type;
    int lineNumber;
    char text[256];
    struct Action *next;
} Action;

Line *createLine(const char *text);
void freeDocument(Document *document);

int pushUndo(Action **top, ActionType type, int number, const char *text);
Action *popUndo(Action **top);
void freeUndoStack(Action **top);
void undo(Document *document, Action **top);

#endif
