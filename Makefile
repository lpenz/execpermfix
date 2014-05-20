prefix=/usr/local
bindir=$(prefix)/bin

all: execpermfix

.PHONY: all install clean

execpermfix: execpermfix.sh
	cp $< $@
	chmod --verbose +x $@

clean:
	rm -f execpermfix

install: execpermfix
	install -d $(DESTDIR)$(bindir)
	install execpermfix $(DESTDIR)$(bindir)/
