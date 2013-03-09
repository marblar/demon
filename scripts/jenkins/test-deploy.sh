#!/bin/sh
starcluster start -c testcluster $BUILD_TAG
starcluster put $BUILD_TAG -u mark scripts/fetchdemon.sh fetchdemon.sh
starcluster sshmaster -u mark ./fetchdemon.sh

# Be sure to execute test-deploy-cleanup.sh to terminate the new cluster.
