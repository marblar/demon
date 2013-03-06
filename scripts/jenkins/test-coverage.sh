#!/bin/bash
autoreconf --install
mkdir -p build
cd build
export OMPI_CXX=clang++
../configure --enable-gcov --with-mpi
make all
make check
make coverage
