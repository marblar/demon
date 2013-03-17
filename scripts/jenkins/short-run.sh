#!/bin/sh
cd $FOLDER
mkdir -p build
cd build
../configure $BOOST_FLAGS
make

INVOCATION="mpirun -np $MAX_PROCESSES ./jdemon-mpi --stoch -n 262144"
OUTPUT_FILE=stoch-result.csv
PLOT_SCRIPT=../scripts/plot/3dscatter.py

# Perform the stochastic run
$INVOCATION | tee $OUTPUT_FILE
python $PLOT_SCRIPT stoch-plot.png  < $OUTPUT_FILE

INVOCATION="mpirun -np $MAX_PROCESSES ./jdemon-mpi --ising -n 4096 -d 20 --clusters 20"
OUTPUT_FILE=ising-result.csv

#Perform the ising run
$INVOCATION | tee $OUTPUT_FILE
python $PLOT_SCRIPT ising-plot.png < $OUTPUT_FILE
