CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDLIBS = -lm

.PHONY: all clean

all: thought-ai

thought-ai: main.c
	$(CC) $(CFLAGS) -o thought-ai main.c $(LDLIBS)

clean:
	rm -f thought-ai
