SHELL = /bin/sh

prefix ?= /usr/local
insdir = $(DESTDIR)$(prefix)/include
srcdir = src

srcfls = $(patsubst $(srcdir)/%,%,$(shell find $(srcdir) -name *.hxx))
dstdrs = $(patsubst $(srcdir)/%,$(insdir)/%,$(shell find $(srcdir) -type d))


check:
	@$(MAKE) -C test check
	@$(MAKE) -C examples check
check-json:
	@$(MAKE) -C test check-json
check-json-node:
	@$(MAKE) -C test check-json-node


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


build:
	@$(MAKE) -C test build
build-json:
	@$(MAKE) -C test build-json
build-json-node:
	@$(MAKE) -C test build-json-node


install:
	@install -d $(dstdrs)
	@for fl in $(srcfls) ; do \
		install -p -m 0644 $(srcdir)/$$fl $(insdir)/$$fl ; \
	done

uninstall:
	@rm -fr $(insdir)/cxon

clean:
	@$(MAKE) -C test clean
	@$(MAKE) -C examples clean


.PHONY: check check-json check-json-node \
        time time-json time-json-node time-json-native \
        build build-json build-json-node  \
        install uninstall \
        clean
