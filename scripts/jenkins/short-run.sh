#!/bin/sh

# Fetches the only artifiact from the upstream job and pipe it into tar xz.
# I hope it's a tarball.
UPSTREAM_FILENAME=`curl "${JENKINS_URL}job/$UPSTREAM_BUILD_NAME/api/xml?depth=1&xpath=/freeStyleProject/build%5Bnumber=$UPSTREAM_BUILD_NUMBER%5D/artifact/relativePath/text()"`
TARBALL_URL=${JENKINS_URL}job/$UPSTREAM_BUILD_NAME/$UPSTREAM_BUILD_NUMBER/artifact/$UPSTREAM_FILENAME
curl $TARBALL_URL | tar xz

cd jdemon-*
mkdir -p build
cd build
../configure --with-boost=$BOOST_ROOT
make
mpirun -np $NODE_FREE_PROCESSES jdemon-mpi $ARGS
