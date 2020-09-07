SHELL = /bin/sh

outdir = .out
incdir = ../src
srcdir = src
datdir = ../data

check-json-src = \
    $(srcdir)/json/json.cxx \
    $(srcdir)/json/fundamental.cxx \
    $(srcdir)/json/compound.cxx \
    $(srcdir)/json/std.cxx

check-json-node-src = \
    $(srcdir)/json/node.cxx

check-cbor-src = \
    $(srcdir)/cbor/cbor.cxx \
    $(srcdir)/cbor/fundamental.cxx

check: check-json check-json-node check-cbor

check-json: $(outdir)/check-json
	@(cd $(outdir) && ../$<)

check-json-node: $(outdir)/check-json-node
	@(cd $(outdir) && ../$<)
	@(cd $(outdir) && ../$< \
                            pass    @$(datdir)/set.1-pass.in \
                                    @$(datdir)/set.2-pass.in \
                            fail    @$(datdir)/set.1-fail.in \
                                    @$(datdir)/set.2-fail.in)
	@(cd $(outdir) && ../$< \
                            diff    @$(datdir)/set.3-diff.in \
                                    @$(datdir)/set.4-diff.in \
                                    @$(datdir)/set.5-diff.in \
                                    | xargs -n2 diff -q)

check-cbor: $(outdir)/check-cbor
	@(cd $(outdir) && ../$<)

$(outdir)/check-json: $(check-json-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-src) -o $@

$(outdir)/check-json-node: $(check-json-node-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-node-src) -o $@

$(outdir)/check-cbor: $(check-cbor-src)
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-cbor-src) -o $@

clean:
	@rm -fr $(outdir)

.PHONY: check check-json check-json-node check-cbor clean
