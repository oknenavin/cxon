SHELL = /bin/sh

INCLUDE ?= ../src

incdir = $(foreach inc,$(INCLUDE),$(addprefix -I,$(inc)))
outdir = .out
SRC = $(wildcard *.cxx)
BIN = $(patsubst %.cxx,$(outdir)/%,$(SRC))

check: $(BIN)

$(outdir)/%: %.cxx
	@mkdir -p $(outdir)
	$(CXX) $(incdir) -O3 $(CPPFLAGS) -std=c++14 -Wall $(CXXFLAGS) $(LDFLAGS) $< -o $@
	@$@ || (echo $@ failed; exit -1)

clean:
	@rm -fr $(outdir)

.PHONY: check clean
