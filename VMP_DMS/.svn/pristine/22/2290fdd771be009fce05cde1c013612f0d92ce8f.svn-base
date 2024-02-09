#!/bin/sh
if [ "x${LD_LIBRARY_PATH}" = "x" ] ; then LD_LIBRARY_PATH="." ; else LD_LIBRARY_PATH=".":${LD_LIBRARY_PATH} ; fi
DIRNAME="`dirname $0`"
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DIRNAME}/../../../BFC_Light
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DIRNAME}/../../../BFC_VFS
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DIRNAME}/../../../BFC_IO
PROGNAME="$0"
${PROGNAME%.sh} "$@"
