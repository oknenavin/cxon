SHELL = /bin/sh

out = .out
src = ../src/cxon
dat = ../data

cxon-json-check-src			= cxon.cxx cxon.fundamental.cxx cxon.compound.cxx cxon.std.cxx
cxon-json-node-check-src		= cxon.json.node.cxx

check: check-cxon-json check-cxon-json-node

check-cxon-json: $(out)/cxon-json-check
	@(cd $(out) && ../$<)

check-cxon-json-node: $(out)/cxon-json-node-check
	@(cd $(out) && ../$<)
	@(cd $(out) && ../$< \
                            pass    @$(dat)/set.1-pass.in \
                                    @$(dat)/set.2-pass.in \
                            fail    @$(dat)/set.1-fail.in \
                                    @$(dat)/set.2-fail.in)
	@(cd $(out) && ../$< \
                            diff    @$(dat)/set.3-diff.in \
                                    @$(dat)/set.4-diff.in \
                                    @$(dat)/set.5-diff.in \
                                    | xargs -n2 diff -q)

$(out)/cxon-json-check: $(cxon-json-check-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxon-json-check-src) -o $@

$(out)/cxon-json-node-check: $(cxon-json-node-check-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxon-json-node-check-src) -o $@
    
clean:
	@rm -fr $(out)

.PHONY: check check-cxon-json check-cxon-json-node clean
