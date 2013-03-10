#!/bin/sh

# Test the URL before we start the cluster.

UPSTREAM_FILENAME=`curl "${JENKINS_URL}job/$UPSTREAM_BUILD_NAME/api/xml?depth=1&xpath=/freeStyleProject/build%5Bnumber=$UPSTREAM_BUILD_NUMBER%5D/artifact/relativePath/text()"`
TARBALL_URL=${JENKINS_URL}job/$UPSTREAM_BUILD_NAME/$UPSTREAM_BUILD_NUMBER/artifact/$UPSTREAM_FILENAME

curl $TARBALL_URL | tar xz

starcluster start -c testcluster --force-spot-master -b .005 $BUILD_TAG
starcluster put $BUILD_TAG -u mark scripts/fetchdemon.sh fetchdemon.sh
starcluster sshmaster $BUILD_TAG -u mark ./fetchdemon.sh $TARBALL_URL

# Be sure to execute test-deploy-cleanup.sh to terminate the new cluster.
