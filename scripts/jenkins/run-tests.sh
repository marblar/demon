#!/bin/bash
autoreconf --install
mkdir -p build
cd build
../configure --enable-gcov
make check
