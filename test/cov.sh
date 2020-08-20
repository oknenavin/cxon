#!/bin/bash -x

rm -rf .cov
rm -f *.gc?? *.info
make clean && make -j4 CXXFLAGS=--coverage check
.lcov-1.14/bin/lcov -c -d . -o cov.info
.lcov-1.14/bin/lcov -e cov.info '*/cxon/*.hxx' -o cxon.info
.lcov-1.14/bin/lcov -l cxon.info
.lcov-1.14/bin/genhtml --demangle-cpp cxon.info -o .cov
