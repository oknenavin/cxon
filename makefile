SHELL = /bin/sh

prefix = /usr/local
includedir = $(prefix)/include
src = src/cxon

cxon-lang-common = \
    $(src)/lang/common/charconv.hxx

cxon-lang-common-chio = \
    $(src)/lang/common/chio/char.hxx \
    $(src)/lang/common/chio/chcls.hxx \
    $(src)/lang/common/chio/chio.hxx \
    $(src)/lang/common/chio/class.hxx \
    $(src)/lang/common/chio/container.hxx \
    $(src)/lang/common/chio/enum.hxx \
    $(src)/lang/common/chio/error.hxx \
    $(src)/lang/common/chio/io.hxx \
    $(src)/lang/common/chio/key.hxx \
    $(src)/lang/common/chio/numeric.hxx \
    $(src)/lang/common/chio/string.hxx \
    $(src)/lang/common/chio/value.hxx

cxon-lang-json-lib-std-bits = \
    $(src)/lang/json/lib/std/bits/map.hxx \
    $(src)/lang/json/lib/std/bits/set.hxx \
    $(src)/lang/json/lib/std/bits/string.hxx

cxon-lang-json-lib-std = \
    $(src)/lang/json/lib/std/array.hxx \
    $(src)/lang/json/lib/std/bitset.hxx \
    $(src)/lang/json/lib/std/chrono.hxx \
    $(src)/lang/json/lib/std/complex.hxx \
    $(src)/lang/json/lib/std/deque.hxx \
    $(src)/lang/json/lib/std/forward_list.hxx \
    $(src)/lang/json/lib/std/list.hxx \
    $(src)/lang/json/lib/std/map.hxx \
    $(src)/lang/json/lib/std/optional.hxx \
    $(src)/lang/json/lib/std/queue.hxx \
    $(src)/lang/json/lib/std/set.hxx \
    $(src)/lang/json/lib/std/stack.hxx \
    $(src)/lang/json/lib/std/string.hxx \
    $(src)/lang/json/lib/std/string_view.hxx \
    $(src)/lang/json/lib/std/tuple.hxx \
    $(src)/lang/json/lib/std/unordered_map.hxx \
    $(src)/lang/json/lib/std/unordered_set.hxx \
    $(src)/lang/json/lib/std/utility.hxx \
    $(src)/lang/json/lib/std/valarray.hxx \
    $(src)/lang/json/lib/std/variant.hxx \
    $(src)/lang/json/lib/std/vector.hxx

cxon-lang-json-node = \
    $(src)/lang/json/node/node.hxx

cxon-lang-json = \
    $(src)/lang/json/class.hxx \
    $(src)/lang/json/compound.hxx \
    $(src)/lang/json/enum.hxx \
    $(src)/lang/json/fundamental.hxx \
    $(src)/lang/json/json.hxx \
    $(src)/lang/json/node.hxx

cxon-json = \
    $(src)/json.hxx

cxon-lib-std = \
    $(src)/lib/std/array.hxx \
    $(src)/lib/std/bitset.hxx \
    $(src)/lib/std/chrono.hxx \
    $(src)/lib/std/complex.hxx \
    $(src)/lib/std/deque.hxx \
    $(src)/lib/std/forward_list.hxx \
    $(src)/lib/std/list.hxx \
    $(src)/lib/std/map.hxx \
    $(src)/lib/std/optional.hxx \
    $(src)/lib/std/queue.hxx \
    $(src)/lib/std/set.hxx \
    $(src)/lib/std/stack.hxx \
    $(src)/lib/std/string.hxx \
    $(src)/lib/std/string_view.hxx \
    $(src)/lib/std/tuple.hxx \
    $(src)/lib/std/unordered_map.hxx \
    $(src)/lib/std/unordered_set.hxx \
    $(src)/lib/std/utility.hxx \
    $(src)/lib/std/valarray.hxx \
    $(src)/lib/std/variant.hxx \
    $(src)/lib/std/vector.hxx

cxon = \
    $(src)/cxon.hxx \
    $(src)/utility.hxx

check:
	@$(MAKE) -C test check

check-json:
	@$(MAKE) -C test check-json

check-json-node:
	@$(MAKE) -C test check-json-node

install: install-json

install-cxon:
	@install -d $(DESTDIR)$(includedir)/cxon/lib/std
	@install -p -m 0644 $(cxon-lib-std)                 $(DESTDIR)$(includedir)/cxon/lib/std
	@install -p -m 0644 $(cxon)                         $(DESTDIR)$(includedir)/cxon

install-common: install-cxon
	@install -d $(DESTDIR)$(includedir)/cxon/lang/common/chio
	@install -p -m 0644 $(cxon-lang-common-chio)        $(DESTDIR)$(includedir)/cxon/lang/common/chio
	@install -p -m 0644 $(cxon-lang-common)             $(DESTDIR)$(includedir)/cxon/lang/common

install-json: install-cxon install-common
	@install -d $(DESTDIR)$(includedir)/cxon/lang/json/lib/std/bits $(DESTDIR)$(includedir)/cxon/lang/json/node
	@install -p -m 0644 $(cxon-lang-json-lib-std-bits)  $(DESTDIR)$(includedir)/cxon/lang/json/lib/std/bits
	@install -p -m 0644 $(cxon-lang-json-lib-std)       $(DESTDIR)$(includedir)/cxon/lang/json/lib/std
	@install -p -m 0644 $(cxon-lang-json-node)          $(DESTDIR)$(includedir)/cxon/lang/json/node
	@install -p -m 0644 $(cxon-lang-json)               $(DESTDIR)$(includedir)/cxon/lang/json
	@install -p -m 0644 $(cxon-json)                    $(DESTDIR)$(includedir)/cxon

uninstall:
	@rm -fr $(DESTDIR)$(includedir)/cxon

clean:
	@$(MAKE) -C test clean

.PHONY: check check-json check-json-node install install-cxon install-common install-json uninstall clean
