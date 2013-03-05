#!/bin/bash
autoreconf --install -I /usr/share/aclocal/
mkdir -p build
cd build
../configure
make distcheck
make dist
