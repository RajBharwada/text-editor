CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

texteditor: texteditor.c
	$(CC) $(CFLAGS) texteditor.c -o texteditor

.PHONY: clean
clean:
	$(RM) texteditor
