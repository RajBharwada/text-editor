#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

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

/* Read one complete line. Reject and consume excess input, never truncate it.
   Return 1 for success, 0 for EOF, and -1 for an oversized line. */
static int readText(FILE *stream, char *text, int size)
{
    int ch;
    size_t length;
    if (fgets(text, size, stream) == NULL)
        return 0;
    length = strlen(text);
    if (length > 0 && text[length - 1] == '\n') {
        text[--length] = '\0';
    } else {
        ch = fgetc(stream);
        if (ch != '\n' && ch != EOF) {
            while ((ch = fgetc(stream)) != '\n' && ch != EOF) { }
            return -1;
        }
    }
    if (length > 0 && text[length - 1] == '\r')
        text[length - 1] = '\0';
    return 1;
}

static int promptText(const char *prompt, char *text, int size)
{
    int result;
    printf("%s", prompt);
    fflush(stdout);
    result = readText(stdin, text, size);
    if (result < 0)
        puts("Input is too long. Operation cancelled.");
    return result;
}

static int promptNumber(const char *prompt, int *number)
{
    char input[256];
    char *end;
    long value;
    int result = promptText(prompt, input, sizeof input);
    if (result != 1)
        return result;
    errno = 0;
    value = strtol(input, &end, 10);
    if (end == input) {
        puts("Please enter a valid number.");
        return -1;
    }
    while (*end == ' ' || *end == '\t')
        end++;
    if (*end != '\0' || errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        puts("Please enter a valid number.");
        return -1;
    }
    *number = (int)value;
    return 1;
}

static Line *getLine(const Document *document, int number)
{
    Line *line = document->head;
    if (number < 1 || number > document->count)
        return NULL;
    while (--number > 0)
        line = line->next;
    return line;
}

static Line *createLine(const char *text)
{
    Line *line = malloc(sizeof *line);
    if (line != NULL) {
        strcpy(line->text, text);
        line->prev = NULL;
        line->next = NULL;
    }
    return line;
}

/* Locate the position, then connect both forward and backward links. */
static int insertLine(Document *document, int number, const char *text)
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

/* Reconnect the neighbors before freeing the removed node. */
static void deleteLine(Document *document, Line *line)
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

static void displayDocument(const Document *document)
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

/* The top of a linked-list stack is its head: push and pop take O(1). */
static int pushUndo(Action **top, ActionType type, int number, const char *text)
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

static Action *popUndo(Action **top)
{
    Action *action = *top;
    if (action != NULL)
        *top = action->next;
    return action;
}

static void freeUndoStack(Action **top)
{
    Action *action;
    while ((action = popUndo(top)) != NULL)
        free(action);
}

static void freeDocument(Document *document)
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

static void undo(Document *document, Action **top)
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

static void changeDocument(Document *document, Action **top, int choice)
{
    int number;
    char text[256];
    Line *line;
    ActionType type;
    if (promptNumber("Enter line position (1-based): ", &number) != 1)
        return;
    line = getLine(document, number);
    if ((choice == 2 && (document->count == INT_MAX || number < 1 || number - 1 > document->count)) ||
        (choice != 2 && line == NULL)) {
        puts("Invalid line position.");
        return;
    }
    if (choice != 3 && promptText("Enter text (up to 255 characters): ", text, sizeof text) != 1)
        return;
    type = choice == 2 ? ACTION_INSERT : (choice == 3 ? ACTION_DELETE : ACTION_EDIT);
    /* Reserve undo memory before changing anything. */
    if (!pushUndo(top, type, number, choice == 2 ? "" : line->text)) {
        puts("Out of memory. Document unchanged.");
        return;
    }
    if (choice == 2) {
        if (!insertLine(document, number, text)) {
            free(popUndo(top));
            puts("Out of memory. Document unchanged.");
            return;
        }
    } else if (choice == 3) {
        deleteLine(document, line);
    } else {
        strcpy(line->text, text);
    }
    puts("Document updated.");
}

static void openFile(Document *document, Action **top)
{
    char filename[256];
    char text[256];
    FILE *file;
    Document loaded = { NULL, NULL, 0 };
    int result;
    int success = 1;
    if (promptText("Enter filename: ", filename, sizeof filename) != 1)
        return;
    file = fopen(filename, "r");
    if (file == NULL) {
        puts("Could not open file.");
        return;
    }
    /* Build a temporary list so a failed read preserves the old document. */
    while ((result = readText(file, text, sizeof text)) == 1) {
        if (loaded.count == INT_MAX || !insertLine(&loaded, loaded.count + 1, text)) {
            puts("Out of memory or too many lines.");
            success = 0;
            break;
        }
    }
    if (result < 0) {
        puts("File contains a line longer than 255 characters.");
        success = 0;
    }
    if (ferror(file)) {
        puts("Could not read file.");
        success = 0;
    }
    if (fclose(file) != 0)
        success = 0;
    if (!success) {
        freeDocument(&loaded);
        puts("Document unchanged.");
        return;
    }
    freeDocument(document);
    freeUndoStack(top);
    *document = loaded;
    puts("File opened. Undo history cleared.");
}

static void saveFile(const Document *document)
{
    char filename[256];
    FILE *file;
    Line *line;
    int success = 1;
    if (promptText("Enter filename (existing files will be overwritten): ", filename, sizeof filename) != 1)
        return;
    file = fopen(filename, "w");
    if (file == NULL) {
        puts("Could not open file for saving.");
        return;
    }
    for (line = document->head; line != NULL; line = line->next) {
        if (fputs(line->text, file) == EOF || fputc('\n', file) == EOF) {
            success = 0;
            break;
        }
    }
    if (fclose(file) != 0)
        success = 0;
    puts(success ? "File saved." : "Could not finish saving file.");
}

int main(void)
{
    Document document = { NULL, NULL, 0 };
    Action *undoStack = NULL;
    int choice;
    int result;
    for (;;) {
        puts("\n========== TEXT EDITOR ==========\n"
             "1. Display document\n2. Insert line\n3. Delete line\n"
             "4. Edit line\n5. Undo\n6. Open file\n7. Save file\n8. Quit");
        result = promptNumber("Enter choice: ", &choice);
        if (result == 0 || (result == 1 && choice == 8))
            break;
        if (result != 1)
            continue;
        switch (choice) {
        case 1: displayDocument(&document); break;
        case 2: case 3: case 4: changeDocument(&document, &undoStack, choice); break;
        case 5: undo(&document, &undoStack); break;
        case 6: openFile(&document, &undoStack); break;
        case 7: saveFile(&document); break;
        default: puts("Invalid menu choice."); break;
        }
    }
    freeDocument(&document);
    freeUndoStack(&undoStack);
    puts("Goodbye.");
    return 0;
}
