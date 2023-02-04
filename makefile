SHELL = /bin/sh

prefix ?= /usr/local
insdir = $(DESTDIR)$(prefix)/include
srcdir = src

srcfls = $(patsubst $(srcdir)/%,%,$(shell find $(srcdir) -name *.hxx))
dstdrs = $(patsubst $(srcdir)/%,$(insdir)/%,$(shell find $(srcdir) -type d))


check:
	@$(MAKE) -C test check
	@$(MAKE) -C example check
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
time-json-s1:
	@$(MAKE) -C test time-json-s1
time-json-s2:
	@$(MAKE) -C test time-json-s2
time-json-node-s1:
	@$(MAKE) -C test time-json-node-s1
time-json-node-s2:
	@$(MAKE) -C test time-json-node-s2
time-json-native-s1:
	@$(MAKE) -C test time-json-native-s1
time-json-native-s2:
	@$(MAKE) -C test time-json-native-s2
time-cbor:
	@$(MAKE) -C test time-cbor


build:
	@$(MAKE) -C test build
build-json:
	@$(MAKE) -C test build-json
build-json-node:
	@$(MAKE) -C test build-json-node
build-cbor:
	@$(MAKE) -C test build-cbor
build-cbor-node:
	@$(MAKE) -C test build-cbor-node


install:
	@install -d $(dstdrs)
	@for fl in $(srcfls) ; do \
		install -p -m 0644 $(srcdir)/$$fl $(insdir)/$$fl ; \
	done

uninstall:
	@rm -fr $(insdir)/cxon

clean:
	@$(MAKE) -C test clean
	@$(MAKE) -C example clean


.PHONY: check check-json check-json-node check-cbor check-cbor-node \
        time time-json time-json-node time-json-native time-cbor \
        build build-json build-json-node build-cbor build-cbor-node \
        install uninstall \
        clean
