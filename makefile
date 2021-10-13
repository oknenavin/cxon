SHELL = /bin/sh

prefix ?= /usr/local
insdir = $(DESTDIR)$(prefix)/include
srcdir = src/cxon

cxon-lang-common = \
    $(srcdir)/lang/common/allocator.hxx \
    $(srcdir)/lang/common/charconv.hxx \
    $(srcdir)/lang/common/container.hxx \
    $(srcdir)/lang/common/hash.hxx \
    $(srcdir)/lang/common/node-value.hxx

cxon-lang-common-cio = \
    $(srcdir)/lang/common/cio/char.hxx \
    $(srcdir)/lang/common/cio/chcls.hxx \
    $(srcdir)/lang/common/cio/cio.hxx \
    $(srcdir)/lang/common/cio/class.hxx \
    $(srcdir)/lang/common/cio/container.hxx \
    $(srcdir)/lang/common/cio/enum.hxx \
    $(srcdir)/lang/common/cio/io.hxx \
    $(srcdir)/lang/common/cio/key.hxx \
    $(srcdir)/lang/common/cio/numeric.hxx \
    $(srcdir)/lang/common/cio/string.hxx \
    $(srcdir)/lang/common/cio/value.hxx

cxon-lang-common-bio = \
    $(srcdir)/lang/common/bio/bio.hxx \
    $(srcdir)/lang/common/bio/io.hxx

cxon-lang-json = \
    $(srcdir)/lang/json/class.hxx \
    $(srcdir)/lang/json/compound.hxx \
    $(srcdir)/lang/json/enum.hxx \
    $(srcdir)/lang/json/error.hxx \
    $(srcdir)/lang/json/fundamental.hxx \
    $(srcdir)/lang/json/json.hxx \
    $(srcdir)/lang/json/json-fwd.hxx \
    $(srcdir)/lang/json/tidy.hxx

cxon-lang-json-lib-std-imp = \
    $(srcdir)/lang/json/lib/std/imp/map.hxx \
    $(srcdir)/lang/json/lib/std/imp/set.hxx

cxon-lang-json-lib-std = \
    $(srcdir)/lang/json/lib/std/array.hxx \
    $(srcdir)/lang/json/lib/std/bitset.hxx \
    $(srcdir)/lang/json/lib/std/chrono.hxx \
    $(srcdir)/lang/json/lib/std/complex.hxx \
    $(srcdir)/lang/json/lib/std/deque.hxx \
    $(srcdir)/lang/json/lib/std/forward_list.hxx \
    $(srcdir)/lang/json/lib/std/list.hxx \
    $(srcdir)/lang/json/lib/std/map.hxx \
    $(srcdir)/lang/json/lib/std/optional.hxx \
    $(srcdir)/lang/json/lib/std/queue.hxx \
    $(srcdir)/lang/json/lib/std/set.hxx \
    $(srcdir)/lang/json/lib/std/stack.hxx \
    $(srcdir)/lang/json/lib/std/string.hxx \
    $(srcdir)/lang/json/lib/std/string_view.hxx \
    $(srcdir)/lang/json/lib/std/tuple.hxx \
    $(srcdir)/lang/json/lib/std/unordered_map.hxx \
    $(srcdir)/lang/json/lib/std/unordered_set.hxx \
    $(srcdir)/lang/json/lib/std/utility.hxx \
    $(srcdir)/lang/json/lib/std/valarray.hxx \
    $(srcdir)/lang/json/lib/std/variant.hxx \
    $(srcdir)/lang/json/lib/std/vector.hxx

cxon-lang-json-node = \
    $(srcdir)/lang/json/node/node.hxx

cxon-lang-cbor-common = \
    $(srcdir)/lang/cbor/common/container.hxx \
    $(srcdir)/lang/cbor/common/sink.hxx \
    $(srcdir)/lang/cbor/common/tag.hxx

cxon-lang-cbor = \
    $(srcdir)/lang/cbor/cbor.hxx \
    $(srcdir)/lang/cbor/cbor-fwd.hxx \
    $(srcdir)/lang/cbor/class.hxx \
    $(srcdir)/lang/cbor/compound.hxx \
    $(srcdir)/lang/cbor/enum.hxx \
    $(srcdir)/lang/cbor/error.hxx \
    $(srcdir)/lang/cbor/fundamental.hxx

cxon-lang-cbor-lib-std-imp = \
    $(srcdir)/lang/cbor/lib/std/imp/map.hxx \
    $(srcdir)/lang/cbor/lib/std/imp/set.hxx

cxon-lang-cbor-lib-std = \
    $(srcdir)/lang/cbor/lib/std/array.hxx \
    $(srcdir)/lang/cbor/lib/std/bitset.hxx \
    $(srcdir)/lang/cbor/lib/std/chrono.hxx \
    $(srcdir)/lang/cbor/lib/std/complex.hxx \
    $(srcdir)/lang/cbor/lib/std/deque.hxx \
    $(srcdir)/lang/cbor/lib/std/forward_list.hxx \
    $(srcdir)/lang/cbor/lib/std/list.hxx \
    $(srcdir)/lang/cbor/lib/std/map.hxx \
    $(srcdir)/lang/cbor/lib/std/optional.hxx \
    $(srcdir)/lang/cbor/lib/std/queue.hxx \
    $(srcdir)/lang/cbor/lib/std/set.hxx \
    $(srcdir)/lang/cbor/lib/std/stack.hxx \
    $(srcdir)/lang/cbor/lib/std/string.hxx \
    $(srcdir)/lang/cbor/lib/std/string_view.hxx \
    $(srcdir)/lang/cbor/lib/std/tuple.hxx \
    $(srcdir)/lang/cbor/lib/std/unordered_map.hxx \
    $(srcdir)/lang/cbor/lib/std/unordered_set.hxx \
    $(srcdir)/lang/cbor/lib/std/utility.hxx \
    $(srcdir)/lang/cbor/lib/std/valarray.hxx \
    $(srcdir)/lang/cbor/lib/std/variant.hxx \
    $(srcdir)/lang/cbor/lib/std/vector.hxx

cxon-lang-cbor-node = \
    $(srcdir)/lang/cbor/node/node.hxx

cxon-lang = \
    $(srcdir)/json.hxx \
    $(srcdir)/cbor.hxx

cxon-lib-std = \
    $(srcdir)/lib/std/array.hxx \
    $(srcdir)/lib/std/bitset.hxx \
    $(srcdir)/lib/std/chrono.hxx \
    $(srcdir)/lib/std/complex.hxx \
    $(srcdir)/lib/std/deque.hxx \
    $(srcdir)/lib/std/forward_list.hxx \
    $(srcdir)/lib/std/list.hxx \
    $(srcdir)/lib/std/map.hxx \
    $(srcdir)/lib/std/optional.hxx \
    $(srcdir)/lib/std/queue.hxx \
    $(srcdir)/lib/std/set.hxx \
    $(srcdir)/lib/std/stack.hxx \
    $(srcdir)/lib/std/string.hxx \
    $(srcdir)/lib/std/string_view.hxx \
    $(srcdir)/lib/std/tuple.hxx \
    $(srcdir)/lib/std/unordered_map.hxx \
    $(srcdir)/lib/std/unordered_set.hxx \
    $(srcdir)/lib/std/utility.hxx \
    $(srcdir)/lib/std/valarray.hxx \
    $(srcdir)/lib/std/variant.hxx \
    $(srcdir)/lib/std/vector.hxx

cxon-lib = \
    $(srcdir)/lib/node.hxx \
    $(srcdir)/lib/node.ordered.hxx

cxon = \
    $(srcdir)/cxon.hxx \
    $(srcdir)/utility.hxx

check:
	@$(MAKE) -C test check

check-json:
	@$(MAKE) -C test check-json

check-json-node:
	@$(MAKE) -C test check-json-node

check-cbor:
	@$(MAKE) -C test check-cbor

check-cbor-node:
	@$(MAKE) -C test check-cbor-node

time:
	@$(MAKE) -C test time

time-json:
	@$(MAKE) -C test time-json

time-cbor:
	@$(MAKE) -C test time-cbor

install: install-lang

install-cxon:
	@install -d                                         $(insdir)/cxon/lib/std
	@install -p -m 0644 $(cxon)                         $(insdir)/cxon
	@install -p -m 0644 $(cxon-lib)                     $(insdir)/cxon/lib
	@install -p -m 0644 $(cxon-lib-std)                 $(insdir)/cxon/lib/std

install-common: install-cxon
	@install -d                                         $(insdir)/cxon/lang/common/cio
	@install -p -m 0644 $(cxon-lang-common-cio)         $(insdir)/cxon/lang/common/cio
	@install -d                                         $(insdir)/cxon/lang/common/bio
	@install -p -m 0644 $(cxon-lang-common)             $(insdir)/cxon/lang/common
	@install -p -m 0644 $(cxon-lang-common-bio)         $(insdir)/cxon/lang/common/bio

install-lang: install-cxon install-common
	@install -p -m 0644 $(cxon-lang)                    $(insdir)/cxon
	@install -d                                         $(insdir)/cxon/lang/json/node
	@install -p -m 0644 $(cxon-lang-json)               $(insdir)/cxon/lang/json
	@install -p -m 0644 $(cxon-lang-json-node)          $(insdir)/cxon/lang/json/node
	@install -d                                         $(insdir)/cxon/lang/json/lib/std/imp
	@install -p -m 0644 $(cxon-lang-json-lib-std)       $(insdir)/cxon/lang/json/lib/std
	@install -p -m 0644 $(cxon-lang-json-lib-std-imp)   $(insdir)/cxon/lang/json/lib/std/imp
	@install -d                                         $(insdir)/cxon/lang/cbor/common
	@install -p -m 0644 $(cxon-lang-cbor-common)        $(insdir)/cxon/lang/cbor/common
	@install -d                                         $(insdir)/cxon/lang/cbor/node
	@install -p -m 0644 $(cxon-lang-cbor)               $(insdir)/cxon/lang/cbor
	@install -p -m 0644 $(cxon-lang-cbor-node)          $(insdir)/cxon/lang/cbor/node
	@install -d                                         $(insdir)/cxon/lang/cbor/lib/std/imp
	@install -p -m 0644 $(cxon-lang-cbor-lib-std)       $(insdir)/cxon/lang/cbor/lib/std
	@install -p -m 0644 $(cxon-lang-cbor-lib-std-imp)   $(insdir)/cxon/lang/cbor/lib/std/imp

uninstall:
	@rm -fr $(insdir)/cxon

clean:
	@$(MAKE) -C test clean

.PHONY: check check-json check-json-node check-cbor install install-cxon install-common install-lang uninstall clean
