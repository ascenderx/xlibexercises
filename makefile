.PHONY: all debug init clean

DEBUGFLAGS =
LINKFLAGS = -lX11
WARNINGS = -Wall -Wextra

all: bin/xlibfun

debug:
	$(MAKE) $(MAKEFILE) DEBUGFLAGS="-DDEBUG -g"

obj/xlibfun.o: src/main.c
	$(CC) -c -o $@ $? $(WARNINGS) $(DEBUGFLAGS)

bin/xlibfun: obj/xlibfun.o
	$(CC) -o $@ $? $(LINKFLAGS)

init:
	mkdir -p obj
	mkdir -p bin

clean:
	rm -rf obj/* bin/*
