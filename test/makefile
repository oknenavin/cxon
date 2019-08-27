SHELL = /bin/sh

out = .out
src = ../src/cxon
dat = ../data

cxon-check-src = cxon.cxx cxon.fundamental.cxx cxon.compound.cxx cxon.std.cxx
cxjson-check-src = cxjson.cxx

check: check-cxon check-cxjson

check-cxon: $(out)/cxon-check
	@(cd $(out) && ../$<)

check-cxjson: $(out)/cxjson-check
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

$(out)/cxon-check: $(cxon-check-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxon-check-src) -o $@

$(out)/cxjson-check: $(cxjson-check-src)
	@mkdir -p $(out)
	$(CXX) -I ../src -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(cxjson-check-src) -o $@
    
clean:
	@rm -fr $(out)

.PHONY: check check-cxon check-cxjson clean
