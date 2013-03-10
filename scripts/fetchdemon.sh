#!/bin/sh
curl http://physics.mblar.us/view/Builds/job/package-demon/lastSuccessfulBuild/artifact/build/demon-0.1.tar.gz | tar xz
cd demon-0.1
mkdir -p build
cd build
../configure
make
qrsh -b y -cwd -pe orte 2 mpirun jdemon-mpi --iterations 1000
