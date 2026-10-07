# Simple Text Editor in C

A menu-driven terminal text editor for a college Data Structures project. It uses standard C and no external libraries. Each document line holds up to 255 characters, including spaces. Positions are numbered from 1; insertion accepts positions from 1 through the current line count plus 1.

## Data structures

- **Doubly Linked List:** each line stores a character array and pointers to its previous and next lines. The document tracks its head, tail, and line count.
- **Stack:** a linked list records insert, delete, and edit actions. Undo removes the latest action and reverses it. Multiple consecutive undos are supported; redo is not included.

## Features

Display, insert, delete, replace a complete line, undo, open a text file, save a text file, and quit. There is no search feature.

Invalid numbers and oversized input are rejected. File opening builds a temporary document, preserving the current document if reading fails or a line exceeds 255 characters. A successful open replaces the document and clears undo history. Saving asks for a filename and overwrites it if it exists. Each saved line ends with a newline. Empty documents produce empty files. Quit or end of input frees both linked lists.

## Compile and run

With GCC installed:

```sh
make
./texteditor
```

Or compile directly:

```sh
gcc -std=c11 -Wall -Wextra -pedantic texteditor.c -o texteditor
./texteditor
```

Remove the local executable with `make clean`.

## Menu

```text
========== TEXT EDITOR ==========
1. Display document
2. Insert line
3. Delete line
4. Edit line
5. Undo
6. Open file
7. Save file
8. Quit

Enter choice:
```

For example, choose `2`, position `1`, and enter `Hello world`. Choose `1` to see `1 | Hello world`. Choose `5` to undo that insertion.

## Algorithms and complexity

Here n is the number of document lines. Character copying is bounded by the fixed line capacity.

| Algorithm / operation | Time |
| --- | --- |
| Linked-list traversal (display or locate a line) | O(n) |
| Linked-list insertion | O(n) to locate; O(1) to connect pointers |
| Linked-list deletion | O(n) to locate; O(1) to unlink pointers |
| Edit / replacement | O(n) to locate |
| Stack push/pop | O(1) |
| Undo | O(n) to locate the affected position |
| Sequential file reading | O(n), appending through the tail |
| Sequential file writing | O(n) |
| Free document / undo stack | O(n) / O(number of recorded actions) |

The code demonstrates linked-list traversal, linked-list insertion, linked-list deletion, stack push/pop, sequential file reading, and sequential file writing. Memory use is proportional to document lines plus recorded actions. `texteditor.c` contains all structures and functions so the implementation can be read in one place.

For optional memory checking (not a project dependency):

```sh
gcc -std=c11 -Wall -Wextra -pedantic -fsanitize=address,undefined -g texteditor.c -o /tmp/texteditor-asan
/tmp/texteditor-asan
```
