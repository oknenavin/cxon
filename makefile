SHELL = /bin/sh

prefix = /usr/local
includedir = $(prefix)/include
src = src/cxon

cxon =          $(src)/cxon.hxx \
                $(src)/pretty.hxx
cxon_bits =     $(src)/bits/cxon.hxx \
                $(src)/bits/charconv.hxx
cxon_core =     $(src)/core/fundamental.hxx \
                $(src)/core/compound.hxx
cxon_std =      $(src)/std/array.hxx \
                $(src)/std/bitset.hxx \
                $(src)/std/chrono.hxx \
                $(src)/std/complex.hxx \
                $(src)/std/deque.hxx \
                $(src)/std/forward_list.hxx \
                $(src)/std/list.hxx \
                $(src)/std/map.hxx \
                $(src)/std/optional.hxx \
                $(src)/std/queue.hxx \
                $(src)/std/set.hxx \
                $(src)/std/stack.hxx \
                $(src)/std/string.hxx \
                $(src)/std/string_view.hxx \
                $(src)/std/tuple.hxx \
                $(src)/std/unordered_map.hxx \
                $(src)/std/unordered_set.hxx \
                $(src)/std/utility.hxx \
                $(src)/std/valarray.hxx \
                $(src)/std/variant.hxx \
                $(src)/std/vector.hxx
cxon_std_bits = $(src)/std/bits/map.hxx \
                $(src)/std/bits/set.hxx
cxon_cxjson =   $(src)/cxjson/cxjson.hxx \
                $(src)/cxjson/pretty.hxx

check:
	@$(MAKE) -C test check

check-cxon:
	@$(MAKE) -C test check-cxon

check-cxjson:
	@$(MAKE) -C test check-cxjson

install: install-core install-std install-cxjson

install-core:
	@install -d $(DESTDIR)$(includedir)/cxon $(DESTDIR)$(includedir)/cxon/bits $(DESTDIR)$(includedir)/cxon/core
	@install -p -m 0644 $(cxon)          $(DESTDIR)$(includedir)/cxon
	@install -p -m 0644 $(cxon_bits)     $(DESTDIR)$(includedir)/cxon/bits
	@install -p -m 0644 $(cxon_core)     $(DESTDIR)$(includedir)/cxon/core

install-std:
	@install -d $(DESTDIR)$(includedir)/cxon/std  $(DESTDIR)$(includedir)/cxon/std/bits
	@install -p -m 0644 $(cxon_std)      $(DESTDIR)$(includedir)/cxon/std
	@install -p -m 0644 $(cxon_std_bits) $(DESTDIR)$(includedir)/cxon/std/bits

install-cxjson:
	@install -d $(DESTDIR)$(includedir)/cxon/cxjson
	@install -p -m 0644 $(cxon_cxjson)   $(DESTDIR)$(includedir)/cxon/cxjson

uninstall:
	@rm -fr $(DESTDIR)$(includedir)/cxon

uninstall-core:
	@rm -fr $(DESTDIR)$(includedir)/cxon

uninstall-std:
	@rm -fr $(DESTDIR)$(includedir)/cxon/std

uninstall-cxjson:
	@rm -fr $(DESTDIR)$(includedir)/cxon/cxjson

clean:
	@$(MAKE) -C test clean

.PHONY: check check-cxon check-cxjson install uninstall clean
