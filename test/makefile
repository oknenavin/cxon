SHELL = /bin/sh

outdir = .out
incdir = ../src
srcdir = src
datdir = ../data

check-json-src = \
    $(srcdir)/json/json.json.cxx \
    $(srcdir)/json/json.fundamental.cxx \
    $(srcdir)/json/json.compound.cxx \
    $(srcdir)/json/json.std.cxx

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
    $(srcdir)/cbor/cbor.std.cxx

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
	@(cd $(outdir) && ./check-json-node \
                                            pass    @$(datdir)/json/set.1-pass.in \
                                                    @$(datdir)/json/set.2-pass.in \
                                            fail    @$(datdir)/json/set.1-fail.in \
                                                    @$(datdir)/json/set.2-fail.in)
	@(cd $(outdir) && ./check-json-node \
                                            diff    @$(datdir)/json/set.3-diff.in \
                                                    @$(datdir)/json/set.4-diff.in \
                                                    @$(datdir)/json/set.5-diff.in \
                                                    | xargs -n2 diff -q)
check-cbor: build-cbor
	@(cd $(outdir) && ./check-cbor)
check-cbor-node: build-cbor-node
	@(cd $(outdir) && ./check-cbor-node)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/test-vector.in)
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/round-trip.in | xargs -n2 diff -q)


time-json: time-json-node time-json-native
time-json-node: build-json-node
	@(cd $(outdir) && ./check-json-node time-node   @$(datdir)/json/time.in)
time-json-native: build-json-node
	@(cd $(outdir) && ./check-json-node time-native @$(datdir)/json/time.in)
time-cbor: build-cbor-node
	@(cd $(outdir) && ./check-cbor-node $(datdir)/cbor/time.in)


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


.PHONY: check check-json check-json-node check-cbor check-cbor-node time time-json time-json-node time-json-native time-cbor build build-json build-json-node build-cbor build-cbor-node clean
