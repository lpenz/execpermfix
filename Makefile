
DESTDIR=/usr/local
BINDIR=$(DESTDIR)/bin
MANDIR=$(DESTDIR)/share/man

CFLAGS=-Wall -Werror


all: execpermfix execpermfix.1

.PHONY: all install clean

execpermfix: execpermfix.o

execpermfix.o: execpermfix.c

execpermfix.1: manual.t2t
	txt2tags -t man -i $^ -o $@

README.textile: manual.t2t
	txt2tags -t textile -H -i $^ -o $@

clean:
	rm -f execpermfix.o execpermfix

install: execpermfix
	mkdir -p $(BINDIR)
	install execpermfix $(BINDIR)/execpermfix
	mkdir -p $(MANDIR)/man1
	install execpermfix.1 $(MANDIR)/man1/execpermfix.1

