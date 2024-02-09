#!/bin/sh
TMPFILE=/tmp/txconfile
/V2O/bin/get_v2d_streaming_profile > ${TMPFILE}
BW=`grep bandwidth ${TMPFILE} |cut -d "\"" -f 4`
LOWCOMP=`grep low_compression ${TMPFILE} |cut -d "\"" -f 4`
HIGHCOMP=`grep high_compression ${TMPFILE} |cut -d "\"" -f 4`
/V2O/bin/set_tx_connection_parm -bandwidth ${BW} -low_compression ${LOWCOMP} -high_compression ${HIGHCOMP}
rm -f ${TMPFILE}
