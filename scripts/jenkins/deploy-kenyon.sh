#!/bin/sh
cd $FOLDER
mkdir -p build
cd build
../configure $BOOST_FLAGS
make
mpirun -np 16 jdemon-mpi $ARGS | tee result.csv
python ../scripts/plot/3dscatter.py plot.png < result.csv
