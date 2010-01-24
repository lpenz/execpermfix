
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/share/man

CFLAGS=-Wall -Werror


all: execpermfix execpermfix.1

.PHONY: all install clean

execpermfix: execpermfix.o

execpermfix.o: execpermfix.c

execpermfix.1: manual.t2t
	txt2tags -t man -i $^ -o $@

README.textile: manual.t2t
	txt2tags -t html -H -i $^ -o $@
	sed -i -e 's@<B>@**@g' -e 's@</B>@**@g' $@

clean:
	rm -f execpermfix.o execpermfix

install: execpermfix
	install -d $(BINDIR) $(MANDIR)/man1
	install execpermfix $(BINDIR)/execpermfix
	install execpermfix.1 $(MANDIR)/man1/execpermfix.1

