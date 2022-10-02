CC := gcc
CFLAGS := -std=c99 -Wall -Werror

.PHONY: clean

all: server client

clean:
	rm server client

server client: %: %.c
	$(CC) $(CFLAGS) -o $@ $^
