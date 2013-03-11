#!/bin/sh
FOLDER=$(basename $1 .tar.gz)
tar xzf $1
cd $FOLDER
mkdir -p build
cd build
../configure
make
mpirun -np $2 jdemon-mpi ${@:2} | tee result.csv
python ../scripts/plot/3dscatter.py "${FOLDER}:jdemon-mpi ${@:2}" < result.csv
mv result.csv ../..
mv plot.png ../..
