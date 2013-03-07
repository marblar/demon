#!/bin/bash                                                                     
autoreconf --install
mkdir -p build
cd build
../configure --enable-performance-tests
make check
