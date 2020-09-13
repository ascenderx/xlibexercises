.PHONY: all debug init clean

DEBUGFLAGS =
LINKFLAGS = -lX11 -lpthread
WARNINGS = -Wall -Wextra

all: bin/xlibfun

debug:
	$(MAKE) $(MAKEFILE) DEBUGFLAGS="-DDEBUG -g"

obj/xdata.o: src/xdata.c \
src/xdata.h \
src/types.h
	$(CC) -c -o $@ $< $(WARNINGS) $(DEBUGFLAGS)

obj/player.o: src/player.c \
src/player.h \
src/types.h \
src/xdata.h
	$(CC) -c -o $@ $< $(WARNINGS) $(DEBUGFLAGS)

obj/game.o: src/game.c \
src/game.h \
src/types.h \
src/xdata.h \
src/player.h
	$(CC) -c -o $@ $< $(WARNINGS) $(DEBUGFLAGS)

obj/main.o: src/main.c \
src/*.h
	$(CC) -c -o $@ $< $(WARNINGS) $(DEBUGFLAGS)

bin/xlibfun: obj/main.o \
obj/xdata.o \
obj/player.o \
obj/game.o
	$(CC) -o $@ $^ $(LINKFLAGS)

init:
	mkdir -p obj
	mkdir -p bin

clean:
	rm -rf obj/* bin/*
