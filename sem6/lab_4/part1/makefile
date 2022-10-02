CC := gcc
CFLAGS := -Wall -Werror -std=gnu99

TEST_PROG := ~/Projects/cw-1/cmake-build-debug/Gui/Gui

.PHONY: test

all: procinfo.exe

test: procinfo.exe
	./procinfo.exe $(TEST_PROG) out.txt && cat out.txt

procinfo.exe: main.c procinfo.c
	$(CC) $(CFLAGS) -o $@ $^
