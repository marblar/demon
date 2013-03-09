#!/bin/bash                                                                     
autoreconf --install
mkdir -p build
cd build
../configure --enable-performance-tests --cache-file=$HOME/demon-cache
make check
