SHELL = /bin/sh

out = .out

test: test-cxon test-cxjson

test-cxon: $(out)/cxon-test
	@(cd $(out) && ../$<)

test-cxjson: $(out)/cxjson-test
	@(cd $(out) && ../$<    pass    @../data/set.1-pass.in \
                                    @../data/set.2-pass.in \
                            fail    @../data/set.1-fail.in \
                                    @../data/set.2-fail.in)

$(out)/cxon-test: cxon.cxx ../cxon.hxx
	@mkdir -p $(out)
	$(CXX) -I.. -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $< -o $@

$(out)/cxjson-test: cxjson.cxx ../cxon.hxx ../cxjson/cxjson.hxx ../cxjson/pretty.hxx
	@mkdir -p $(out)
	$(CXX) -I.. -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $< -o $@
    
clean:
	@rm -fr $(out)

.PHONY: test test-cxon test-cxjson clean
