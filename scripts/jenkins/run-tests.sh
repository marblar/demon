#!/bin/bash
autoreconf --install
mkdir -p build
cd build

# There's a CPPUNIT  on Natasha when using gcc47, so this is our work-around
# since debugging a unit test framework is not a business requirement.
../configure CXX=clang++ --disable-openmp
make check
