#!/bin/sh
EXEC_NAME=tmgas
./$EXEC_NAME --report_format=xml --report_level=detailed --log_level=test_suite --log_format=xml\
|xsltproc junit.xslt -\
> $EXEC_NAME.stats
