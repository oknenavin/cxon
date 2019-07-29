SHELL = /bin/sh

prefix = /usr/local
includedir = $(prefix)/include
src = src/cxon

cxon_files =    $(src)/cxon.hxx \
                	$(src)/optional.hxx \
                $(src)/variant.hxx \
                $(src)/pretty.hxx \

cxjson_files =  $(src)/cxjson/cxjson.hxx \
                $(src)/cxjson/pretty.hxx

test:
	@$(MAKE) -C test test

test-cxon:
	@$(MAKE) -C test test-cxon

test-cxjson:
	@$(MAKE) -C test test-cxjson

install:
	@install -d $(DESTDIR)$(includedir)/cxon/cxjson
	@install -p -m 0644 $(cxon_files) $(DESTDIR)$(includedir)/cxon
	@install -p -m 0644 $(cxjson_files) $(DESTDIR)$(includedir)/cxon/cxjson

uninstall:
	@rm -fr $(DESTDIR)$(includedir)/cxon

clean:
	@$(MAKE) -C test clean

.PHONY: test test-cxon test-cxjson install uninstall clean
