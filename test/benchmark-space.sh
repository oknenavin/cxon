#!/usr/bin/bash

printf '# %16s%8s%6s\n' ' ' 'size' 'time'

s10=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s10 src/json/space/cxon.native.c0.cxx 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #0)"' `stat -c%s .out/s10` $s10

s11=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s11 src/json/space/cxon.native.c1.cxx 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #1)"' `stat -c%s .out/s11` $s11

s12=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s12 src/json/space/cxon.native.c2.cxx 2>&1`
printf '%-18s%8s%6s\n' '"cxon (native #2)"' `stat -c%s .out/s12` $s12

s13=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s13 src/json/space/cxon.node.cxx 2>&1`
printf '%-18s%8s%6s\n' '"cxon (node)"' `stat -c%s .out/s13` $s13

# TODO: BOOST_JSON_STANDALONE deprecated
/usr/bin/time -f%e -o .out/t20 $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s20  src/json/space/boost.json.cxx 2> /dev/null
printf '%-18s%8s%6s\n' '"boost/json"' `stat -c%s .out/s20` `cat .out/t20`

s30=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s30  src/json/space/rapidjson.cxx 2>&1`
printf '%-18s%8s%6s\n' '"rapidjson"' `stat -c%s .out/s30` $s30

s40=`/usr/bin/time -f%e $CXX -O2 -s -DNDEBUG $CXXFLAGS -I ../src $CPPFLAGS -o .out/s40  src/json/space/nlohmann.json.cxx 2>&1`
printf '%-18s%8s%6s\n' '"nlohmann.json"' `stat -c%s .out/s40` $s40
