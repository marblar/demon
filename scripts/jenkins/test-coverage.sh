#!/bin/bash
autoreconf --install
mkdir -p build
cd build
../configure --enable-gcov --cache-file=$HOME/demon-cache
make -i coverage
