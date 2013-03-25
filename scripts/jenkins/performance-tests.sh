#!/bin/bash                                                                     
autoreconf --install
mkdir -p build
cd build
../configure --enable-performance-tests --cache-file=$HOME/demon-cache
make check
PERFORMANCE_DIR=build/test/performance
test $PERFORMANCE_DIR/ising.stats $PERFORMANCE_DIR/stoch.stats
