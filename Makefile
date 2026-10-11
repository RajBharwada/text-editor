CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
SRC = src/core.c src/edit.c src/delete.c src/navigation.c src/main.c
OBJ = $(SRC:src/%.c=build/%.o)

texteditor: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o texteditor

build/%.o: src/%.c $(wildcard include/*.h)
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r build texteditor
