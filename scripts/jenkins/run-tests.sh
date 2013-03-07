#!/bin/bash
autoreconf --install
mkdir -p build
cd build
../configure
make check
