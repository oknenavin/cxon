#!/bin/bash -x

rm -rf .cov
rm -f *.gc??
rm -f *.info
make clean && make -j4 CXXFLAGS=--coverage test
/usr/local/bin/lcov -c -d . -o cov.info
/usr/local/bin/lcov -e cov.info '*/cxon/*.hxx' -o cxon.info
/usr/local/bin/lcov -l cxon.info
/usr/local/bin/genhtml --demangle-cpp cxon.info -o .cov
