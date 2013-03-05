#!/bin/bash                                                                     
autoreconf --install -I /usr/share/aclocal/
mkdir -p build
cd build
../configure --enable-performance-tests --disable-openmp CXX=clang++
make check
