SHELL = /bin/sh

INCLUDE ?= ../src

incdir = $(foreach inc,$(INCLUDE),$(addprefix -I,$(inc)))
outdir = .out
srcdir = src
datdir = ../data

check-json-src = $(wildcard $(srcdir)/json/json.core-*.cxx)
check-json-obj = $(patsubst %.cxx,$(outdir)/%.o,$(notdir $(check-json-src)))

check-json-node-src = $(wildcard $(srcdir)/json/json.node-*.cxx)
check-json-node-obj = $(patsubst %.cxx,$(outdir)/%.o,$(notdir $(check-json-node-src)))

check-cbor-src = $(wildcard $(srcdir)/cbor/cbor.core-*.cxx)
check-cbor-obj = $(patsubst %.cxx,$(outdir)/%.o,$(notdir $(check-cbor-src)))

check-cbor-node-src = $(wildcard $(srcdir)/cbor/cbor.node-*.cxx)
check-cbor-node-obj = $(patsubst %.cxx,$(outdir)/%.o,$(notdir $(check-cbor-node-src)))


check: check-all
time: time-all
build: build-json build-json-node build-cbor build-cbor-node


check-all: build-json build-json-node build-cbor build-cbor-node
	@(cd $(outdir) && ./check-json)
	@(cd $(outdir) && ./check-json-node)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf pass fail)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf diff-node   | xargs -n2 diff -q)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf diff-native | xargs -n2 diff -q)
	@(cd $(outdir) && ./check-cbor)
	@(cd $(outdir) && ./check-cbor-node)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf test-vector)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf round-trip  | xargs -n2 diff -q)
check-json: build-json
	@(cd $(outdir) && ./check-json)
check-json-node: build-json-node
	@(cd $(outdir) && ./check-json-node)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf pass fail)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf diff-node   | xargs -n2 diff -q)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf diff-native | xargs -n2 diff -q)
check-cbor: build-cbor
	@(cd $(outdir) && ./check-cbor)
check-cbor-node: build-cbor-node
	@(cd $(outdir) && ./check-cbor-node)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf test-vector)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf round-trip | xargs -n2 diff -q)

time-all: build-json-node build-cbor-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s1)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s1)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s2)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s2)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf time)
time-json: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s1)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s1)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s2)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s2)
time-json-s1: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s1)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s1)
time-json-s2: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s2)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s2)
time-json-node-s1: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s1)
time-json-node-s2: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-node-s2)
time-json-native-s1: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s1)
time-json-native-s2: build-json-node
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf time-native-s2)
time-cbor: build-cbor-node
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf time)


build-json: $(outdir)/check-json
build-json-node: $(outdir)/check-json-node
build-cbor: $(outdir)/check-cbor
build-cbor-node: $(outdir)/check-cbor-node

$(outdir)/check-json: $(check-json-obj)
	$(CXX) $(check-json-obj)  $(LDFLAGS)  -o $@
$(outdir)/json.core-%.o: $(srcdir)/json/json.core-%.cxx
	@mkdir -p $(outdir)
	$(CXX) -c $(incdir) -DNDEBUG $(CPPFLAGS) -O3 -std=c++11 -Wall $(CXXFLAGS) $<  -o $@

$(outdir)/check-json-node: $(check-json-node-obj)
	$(CXX) $(check-json-node-obj)  $(LDFLAGS)  -o $@
$(outdir)/json.node-%.o: $(srcdir)/json/json.node-%.cxx
	@mkdir -p $(outdir)
	$(CXX) -c $(incdir) -DNDEBUG $(CPPFLAGS) -O3 -std=c++11 -Wall $(CXXFLAGS) $<  -o $@

$(outdir)/check-cbor: $(check-cbor-obj)
	$(CXX) $(check-cbor-obj)  $(LDFLAGS)  -o $@
$(outdir)/cbor.core-%.o: $(srcdir)/cbor/cbor.core-%.cxx
	@mkdir -p $(outdir)
	$(CXX) -c $(incdir) -DNDEBUG $(CPPFLAGS) -O3 -std=c++11 -Wall $(CXXFLAGS) $<  -o $@

$(outdir)/check-cbor-node: $(check-cbor-node-obj)
	$(CXX) $(check-cbor-node-obj)  $(LDFLAGS)  -o $@
$(outdir)/cbor.node-%.o: $(srcdir)/cbor/cbor.node-%.cxx
	@mkdir -p $(outdir)
	$(CXX) -c $(incdir) -DNDEBUG $(CPPFLAGS) -O3 -std=c++11 -Wall $(CXXFLAGS) $<  -o $@


clean:
	@rm -fr $(outdir)


.PHONY: check check-all check-json check-json-node check-cbor check-cbor-node \
        time time-all time-json time-json-s1 time-json-s2 time-json-node-s1 time-json-native-s1 time-json-node-s2 time-json-native-s2 time-cbor \
        build build-json build-json-node build-cbor build-cbor-node \
        clean
