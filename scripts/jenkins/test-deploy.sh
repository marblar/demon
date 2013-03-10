#!/bin/sh
starcluster start -c testcluster --force-spot-master -b .005 $BUILD_TAG
starcluster put $BUILD_TAG -u mark scripts/fetchdemon.sh fetchdemon.sh
starcluster put $BUILD_TAG -u mark *.tar.gz *.tar.gz

DOWNSTREAM_URL=`curl $JENKINS_URL/job/package-demon/api/xml?depth=1&xpath=/freeStyleProject/build%5Baction/cause/upstreamProject=%22$JOB_NAME%22%20and%20action/cause/upstreamBuild=$BUILD_ID%5D/url/text()`
DOWNSTREAM_FILENAME=`curl $JENKINS_URL/job/package-demon/api/xml?depth=1&xpath=/freeStyleProject/build%5Baction/cause/upstreamProject=%22$JOB_NAME%22%20and%20action/cause/upstreamBuild=$BUILD_ID%5D/artifact/fileName/text()`

TARBALL_URL=$DOWNSTREAM_URL/artifact/build/$DOWNSTREAM_FILENAME

starcluster sshmaster $BUILD_TAG -u mark ./fetchdemon.sh $TARBALL_URL

# Be sure to execute test-deploy-cleanup.sh to terminate the new cluster.
