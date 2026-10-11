/* main.c - Person 5: File Handling + Integration
   Safe input reading, opening/saving files, and the menu loop that connects
   every other module. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "core.h"
#include "edit.h"
#include "delete.h"
#include "navigation.h"
#include "main.h"

/* Read one complete line. Reject and consume excess input, never truncate it.
   Return 1 for success, 0 for EOF, and -1 for an oversized line. */
int readText(FILE *stream, char *text, int size)
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

int promptText(const char *prompt, char *text, int size)
{
    int result;
    printf("%s", prompt);
    fflush(stdout);
    result = readText(stdin, text, size);
    if (result < 0)
        puts("Input is too long. Operation cancelled.");
    return result;
}

int promptNumber(const char *prompt, int *number)
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

void openFile(Document *document, Action **top)
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

void saveFile(const Document *document)
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
        case 2: insertLineCommand(&document, &undoStack); break;
        case 3: deleteLineCommand(&document, &undoStack); break;
        case 4: editLineCommand(&document, &undoStack); break;
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
