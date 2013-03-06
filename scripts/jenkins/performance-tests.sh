#!/bin/bash                                                                     
autoreconf --install
mkdir -p build
cd build
../configure --enable-performance-tests --disable-openmp CXX=clang++
make check
