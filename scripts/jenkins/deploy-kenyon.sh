#!/bin/sh
mkdir -p build
cd build
../configure --with-boost=$BOOST_ROOT
make
mpirun -np 16 jdemon-mpi $ARGS | tee result.csv
python ../scripts/plot/3dscatter.py plot.png < result.csv
