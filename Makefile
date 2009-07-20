
CFLAGS=-Wall -Werror


all: execpermfix

execpermfix: execpermfix.o

execpermfix.o: execpermfix.c

clean:
	rm -f execpermfix.o execpermfix

