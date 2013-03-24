#!/bin/bash
autoreconf --install
mkdir -p build
cd build
../configure --cache-file=$HOME/demon-cache
make check
test test/*.xml
