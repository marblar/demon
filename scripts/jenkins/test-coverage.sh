#!/bin/bash
autoreconf --install -I /usr/share/aclocal/
mkdir -p build
cd build
../configure --enable-gcov
make all
make check
make coverage
