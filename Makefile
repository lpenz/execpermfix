
CFLAGS=-Wall -Werror


all: execpermfix

.PHONY: all clean

execpermfix: execpermfix.o

execpermfix.o: execpermfix.c

clean:
	rm -f execpermfix.o execpermfix

