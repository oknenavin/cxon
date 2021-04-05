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
    $(srcdir)/json/json.node.cxx

check-cbor-src = \
    $(srcdir)/cbor/cbor.cbor.cxx \
    $(srcdir)/cbor/cbor.fundamental.cxx \
    $(srcdir)/cbor/cbor.compound.cxx \
    $(srcdir)/cbor/cbor.std.cxx

check-cbor-node-src = \
    $(srcdir)/cbor/cbor.node.cxx

check: check-json check-json-node check-cbor check-cbor-node

time: time-json

check-json: $(outdir)/check-json
	@(cd $(outdir) && ../$<)

check-json-node: $(outdir)/check-json-node
	@(cd $(outdir) && ../$<)
	@(cd $(outdir) && ../$< \
                            pass    @$(datdir)/json/set.1-pass.in \
                                    @$(datdir)/json/set.2-pass.in \
                            fail    @$(datdir)/json/set.1-fail.in \
                                    @$(datdir)/json/set.2-fail.in)
	@(cd $(outdir) && ../$< \
                            diff    @$(datdir)/json/set.3-diff.in \
                                    @$(datdir)/json/set.4-diff.in \
                                    @$(datdir)/json/set.5-diff.in \
                                    | xargs -n2 diff -q)

time-json: $(outdir)/check-json-node
	@(cd $(outdir) && ../$< time @$(datdir)/json/time.in)

check-cbor: $(outdir)/check-cbor
	@(cd $(outdir) && ../$<)

check-cbor-node: $(outdir)/check-cbor-node
	@(cd $(outdir) && ../$<)
	@(cd $(outdir) && ../$< $(datdir)/cbor/set.1.in)
	@(cd $(outdir) && ../$< $(datdir)/cbor/round-trip.in | xargs -n2 diff -q)

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

.PHONY: check check-json check-json-node check-cbor check-cbor-node time time-json clean
