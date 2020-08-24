SHELL = /bin/sh

out = .out
inc = ../src
src = src
dat = ../data

check-json-src = \
    $(src)/json/json.cxx \
    $(src)/json/fundamental.cxx \
    $(src)/json/compound.cxx \
    $(src)/json/std.cxx

check-json-node-src = \
    $(src)/json/node.cxx

check: check-json check-json-node

check-json: $(out)/check-json
	@(cd $(out) && ../$<)

check-json-node: $(out)/check-json-node
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

$(out)/check-json: $(check-json-src)
	@mkdir -p $(out)
	$(CXX) -I $(inc) -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-src) -o $@

$(out)/check-json-node: $(check-json-node-src)
	@mkdir -p $(out)
	$(CXX) -I $(inc) -DNDEBUG $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $(check-json-node-src) -o $@

clean:
	@rm -fr $(out)

.PHONY: check check-json check-json-node clean
