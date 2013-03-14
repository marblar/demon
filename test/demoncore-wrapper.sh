#!/bin/sh
lib/demoncore --output_format=XML --log_level=all --report_level=no > testresults.xml
test testresults.xml
