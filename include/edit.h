/* edit.h - Person 2: Insertion & Editing */
#ifndef EDIT_H
#define EDIT_H

#include "core.h"

int insertLine(Document *document, int number, const char *text);
void insertLineCommand(Document *document, Action **top);  /* menu option 2 */
void editLineCommand(Document *document, Action **top);    /* menu option 4 */

#endif
