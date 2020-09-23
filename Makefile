CC = gcc # C compiler
CFLAGS = -Wall -Wextra -pedantic -std=c11 # C flags
LDFLAGS = -shared # linking flags
RM = rm -f # rm command
TARGET_LIB = libgol.so # target lib

SHAREDPATH = GameOfLife

all:	libgol.so gameoflife.c
	$(CC) $(CFLAGS) -L../$(SHAREDPATH) -Wall -o gameoflife gameoflife.c -lgol
libgol.so:	gol.o
	$(CC) $(CFLAGS) ${LDFLAGS} -o ${TARGET_LIB} gol.o
gol.o:	gol.c gol.h
	$(CC) $(CFLAGS) -c -fpic gol.c
clean:
	$(RM) gameoflife gol.o


