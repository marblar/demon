#!/bin/bash
AUTOMAKE=automake-1.13
ACLOCAL=aclocal-1.13
autoreconf --install -I /usr/share/aclocal/
mkdir -p build
cd build
../configure --enable-gcov
make check
