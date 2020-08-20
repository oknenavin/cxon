#!/bin/bash -x

rm -rf .bld
rm -rf .out
mkdir .bld && cd .bld
cmake ..
cmake --build . --config Release
ctest -V -C Release
