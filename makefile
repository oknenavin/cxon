SHELL = /bin/sh

prefix = /usr/local
includedir = $(prefix)/include

test:
	@$(MAKE) -C test test

test-cxon:
	@$(MAKE) -C test test-cxon

test-cxjson:
	@$(MAKE) -C test test-cxjson

install:
	@install -d $(DESTDIR)$(includedir)/cxon/cxjson
	@install -p -m 0644 cxon.hxx pretty.hxx $(DESTDIR)$(includedir)/cxon
	@install -p -m 0644 cxjson/cxjson.hxx cxjson/pretty.hxx $(DESTDIR)$(includedir)/cxon/cxjson

uninstall:
	@rm -fr $(DESTDIR)$(includedir)/cxon

clean:
	@$(MAKE) -C test clean

.PHONY: test test-cxon test-cxjson install uninstall clean
