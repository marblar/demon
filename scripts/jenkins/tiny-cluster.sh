#!/bin/sh

# Test the URL before we start the cluster.

starcluster start -c testcluster --force-spot-master -b .005 $BUILD_TAG
starcluster put $BUILD_TAG -u mark $TARBALL $TARBALL
starcluster put $BUILD_TAG -u mark $FOLDER/scripts/baton.sh baton.sh
starcluster sshmaster $BUILD_TAG -u mark "./baton.sh $TARBALL 2 --iterations 1000"
starcluster get $BUILD_TAG -u mark result.csv result.csv
starcluster get $BUILD_TAG -u mark plot.png plot.png

# Be sure to execute tiny-cluster-cleanup.sh to terminate the new cluster.
