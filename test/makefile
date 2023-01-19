SHELL = /bin/sh

outdir = .out
incdir = ../src
srcdir = src
datdir = ../data

check-json-src = \
    $(srcdir)/json/json.json.cxx \
    $(srcdir)/json/json.fundamental.cxx \
    $(srcdir)/json/json.compound.cxx \
    $(srcdir)/json/json.std.cxx \
    $(srcdir)/json/json.boost.cxx

check-json-node-src = \
    $(srcdir)/json/json.node.cxx \
    $(srcdir)/json/json.node.self.cxx \
    $(srcdir)/json/json.node.time.node.cxx \
    $(srcdir)/json/json.node.time.native.cxx \
    $(srcdir)/json/json.node.time.boost.cxx \
    $(srcdir)/json/json.node.time.rapidjson.cxx \
    $(srcdir)/json/json.node.time.nlohmann.cxx

check-cbor-src = \
    $(srcdir)/cbor/cbor.cbor.cxx \
    $(srcdir)/cbor/cbor.fundamental.cxx \
    $(srcdir)/cbor/cbor.compound.cxx \
    $(srcdir)/cbor/cbor.std.cxx \
    $(srcdir)/cbor/cbor.boost.cxx

check-cbor-node-src = \
    $(srcdir)/cbor/cbor.node.cxx \
    $(srcdir)/cbor/cbor.node.self.cxx


check: check-json check-json-node check-cbor check-cbor-node
time: time-json time-cbor
build: build-json build-json-node build-cbor build-cbor-node


check-json: build-json
	@(cd $(outdir) && ./check-json)
check-json-node: build-json-node
	@(cd $(outdir) && ./check-json-node)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf pass fail)
	@(cd $(outdir) && ./check-json-node $(datdir)/json/tests.cf diff | xargs -n2 diff -q)
check-cbor: build-cbor
	@(cd $(outdir) && ./check-cbor)
check-cbor-node: build-cbor-node
	@(cd $(outdir) && ./check-cbor-node)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf test-vector)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/tests.cf round-trip | xargs -n2 diff -q)


time-json: time-json-s1 time-json-s2
time-json-s1: time-json-node-s1 time-json-native-s1
time-json-s2: time-json-node-s2 time-json-native-s2
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

$(outdir)/check-json: $(check-json-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG -O3 $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-src) -o $@

$(outdir)/check-json-node: $(check-json-node-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG -O3 $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-node-src) -o $@

$(outdir)/check-cbor: $(check-cbor-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG -O3 $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-cbor-src) -o $@

$(outdir)/check-cbor-node: $(check-cbor-node-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG -O3 $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-cbor-node-src) -o $@


clean:
	@rm -fr $(outdir)


.PHONY: check check-json check-json-node check-cbor check-cbor-node \
        time time-json time-json-s1 time-json-s2 time-json-node-s1 time-json-native-s1 time-json-node-s2 time-json-native-s2 time-cbor \
        build build-json build-json-node build-cbor build-cbor-node clean
