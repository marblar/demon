#!/bin/bash                                                                     
autoreconf --install
mkdir -p build
cd build
EXPORT OMPI_CXX=clang++
../configure --enable-performance-tests --with-mpi CXX=clang++
make check
