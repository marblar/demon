#!/bin/sh

starcluster start -c deploy aws-deployment 
starcluster put aws-deployment -u mark $TARBALL $TARBALL
starcluster put aws-deployment -u mark $FOLDER/scripts/baton.sh baton.sh 
starcluster sshmaster aws-deployment -u mark "./baton.sh $TARBALL 250 --iterations 8388608"
starcluster get aws-deployment -u mark result.csv result.csv
starcluster get aws-deployment -u mark plot.png plot.png

# Be sure to execute deploy-aws-cleanup.sh **EVEN ON FAILURE** to spin down the cluster.                              

