#!/usr/bin/bash

printf '# %16s%8s%6s\n' ' ' 'size' 'time'

s10=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/cxon.native.c0.cxx $LDFLAGS -o .out/s10 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #0)"' `stat -c%s .out/s10` $s10

s11=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/cxon.native.c1.cxx $LDFLAGS -o .out/s11 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #1)"' `stat -c%s .out/s11` $s11

s12=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/cxon.native.c2.cxx $LDFLAGS -o .out/s12 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #2)"' `stat -c%s .out/s12` $s12

s13=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/cxon.node.cxx $LDFLAGS -o .out/s13 2>&1`
printf '%-18s%8s%6s\n' '"cxon (node)"' `stat -c%s .out/s13` $s13

s20=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/boost.json.cxx $LDFLAGS -o .out/s20 2>&1`
printf '%-18s%8s%6s\n' '"boost/json"' `stat -c%s .out/s20` $s20

s30=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/rapidjson.cxx $LDFLAGS -o .out/s30 2>&1`
printf '%-18s%8s%6s\n' '"rapidjson"' `stat -c%s .out/s30` $s30

s40=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG -I ../src $CPPFLAGS $CXXFLAGS src/json/space/nlohmann.json.cxx $LDFLAGS -o .out/s40 2>&1`
printf '%-18s%8s%6s\n' '"nlohmann.json"' `stat -c%s .out/s40` $s40
