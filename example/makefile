SHELL = /bin/sh

incdir = ../src
outdir = .out
SRC = $(wildcard *.cxx)
BIN = $(patsubst %.cxx,$(outdir)/%,$(SRC))

check: $(BIN)

$(outdir)/%: %.cxx
	@mkdir -p $(outdir)
	$(CXX) -I $(incdir) -O3 $(CPPFLAGS) -std=c++11 -Wall $(CXXFLAGS) $(LDFLAGS) $< -o $@
	@$@ || (echo $@ failed; exit -1)

clean:
	@rm -fr $(outdir)

.PHONY: check clean
