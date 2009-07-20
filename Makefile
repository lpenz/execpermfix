
DESTDIR=/usr/local
BINDIR=$(DESTDIR)/bin

CFLAGS=-Wall -Werror


all: execpermfix

.PHONY: all install clean

execpermfix: execpermfix.o

execpermfix.o: execpermfix.c

clean:
	rm -f execpermfix.o execpermfix

install: execpermfix
	mkdir -p $(BINDIR)
	install execpermfix $(BINDIR)/execpermfix

