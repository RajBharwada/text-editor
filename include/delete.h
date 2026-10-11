/* delete.h - Person 3: Deletion Operations */
#ifndef DELETE_H
#define DELETE_H

#include "core.h"

void deleteLine(Document *document, Line *line);
void deleteLineCommand(Document *document, Action **top);  /* menu option 3 */

#endif
