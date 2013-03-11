#!/bin/sh
FOLDER=$(basename $1 .tar.gz)
tar xzf $1
shift
cd $FOLDER
mkdir -p build
cd build
../configure
make
PROCESSES=$1
shift
mpirun -np $PROCESSES jdemon-mpi $@ | tee result.csv
python ../scripts/plot/3dscatter.py '${FOLDER}:jdemon-mpi $@' < result.csv
mv result.csv ../..
mv plot.png ../..
