/* main.h - Person 5: File Handling + Integration
   Input helpers and file operations shared with the other modules. */
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "core.h"

int readText(FILE *stream, char *text, int size);
int promptText(const char *prompt, char *text, int size);
int promptNumber(const char *prompt, int *number);

void openFile(Document *document, Action **top);
void saveFile(const Document *document);

#endif
