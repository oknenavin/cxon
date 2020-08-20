#!/bin/bash -x

rm -rf .lcov
mkdir .lcov && wget -P .lcov http://downloads.sourceforge.net/ltp/lcov-1.14.tar.gz
tar xvfz .lcov/lcov-1.14.tar.gz -C .lcov
make -C .lcov

rm -f *.gc?? *.info
make clean && make -j4 CXXFLAGS=--coverage check

rm -rf .cov
.lcov/lcov-1.14/bin/lcov --gcov-tool gcov-8 -c -d . -o cov.info
.lcov/lcov-1.14/bin/lcov -e cov.info '*/cxon/*.hxx' -o cxon.info 
.lcov/lcov-1.14/bin/lcov -l cxon.info
