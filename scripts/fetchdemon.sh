#!/bin/sh
curl $1 | tar xz
cd jdemon-*
mkdir -p build
cd build
../configure
make
qrsh -b y -cwd -pe orte 2 mpirun jdemon-mpi --iterations 100
