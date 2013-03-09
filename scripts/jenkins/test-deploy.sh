#!/bin/sh
starcluster start -c testcluster --force-spot-master -b .005 $BUILD_TAG
starcluster put $BUILD_TAG -u mark scripts/fetchdemon.sh fetchdemon.sh
starcluster sshmaster -u mark $BUILD_TAG ./fetchdemon.sh

# Be sure to execute test-deploy-cleanup.sh to terminate the new cluster.
