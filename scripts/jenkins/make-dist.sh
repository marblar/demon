#!/bin/bash
autoreconf --install
mkdir -p build
cd build
../configure CXX=clang++
make distcheck
make dist
