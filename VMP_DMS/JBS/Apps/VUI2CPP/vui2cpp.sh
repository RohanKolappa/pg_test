#!/bin/sh
if [ "x${LD_LIBRARY_PATH}" = "x" ] ; then LD_LIBRARY_PATH="." ; else LD_LIBRARY_PATH=".":${LD_LIBRARY_PATH} ; fi
DIRNAME="`dirname $0`"
PROGNAME="$0"
${PROGNAME%.sh} "$@"
