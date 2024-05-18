CC = gcc
CFLAGS = -Wall -Wextra -std=c11
EXECUTABLE = community.exe

SRCS = $(wildcard *.c)

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(EXECUTABLE)
