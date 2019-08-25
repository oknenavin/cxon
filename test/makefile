SHELL = /bin/sh

out = .out
src = ../src/cxon
dat = ../data

cxon-test-src = cxon.cxx cxon.fundamental.cxx cxon.compound.cxx cxon.std.cxx
cxjson-test-src = cxjson.cxx

test: test-cxon test-cxjson

test-cxon: $(out)/cxon-test
	@(cd $(out) && ../$<)

test-cxjson: $(out)/cxjson-test
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

$(out)/cxon-test: $(cxon-test-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxon-test-src) -o $@

$(out)/cxjson-test: $(cxjson-test-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxjson-test-src) -o $@
    
clean:
	@rm -fr $(out)

.PHONY: test test-cxon test-cxjson clean
