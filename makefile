.PHONY: all debug init clean

DEBUGFLAGS =
LINKFLAGS = -lX11
WARNINGS = -Wall -Wextra

all: bin/xlibfun

debug:
	$(MAKE) $(MAKEFILE) DEBUGFLAGS="-DDEBUG -g"

obj/main.o: src/*.h src/*.c
	$(CC) -c -o $@ src/main.c $(WARNINGS) $(DEBUGFLAGS)

obj/xdata.o: src/types.h src/xdata.h src/xdata.c
	$(CC) -c -o $@ src/xdata.c $(WARNINGS) $(DEBUGFLAGS)

bin/xlibfun: obj/main.o obj/xdata.o
	$(CC) -o $@ obj/*.o $(LINKFLAGS)

init:
	mkdir -p obj
	mkdir -p bin

clean:
	rm -rf obj/* bin/*
