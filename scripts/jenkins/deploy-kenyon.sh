#!/bin/sh
cd $FOLDER
mkdir -p build
cd build
../configure $BOOST_FLAGS
make
mpirun -np $MAX_PROCESSES ./jdemon-mpi $ARGS | tee result.csv
python ../scripts/plot/3dscatter.py plot.png < result.csv
