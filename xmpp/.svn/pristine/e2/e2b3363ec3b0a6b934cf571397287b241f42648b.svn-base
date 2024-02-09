#!/bin/sh


#help 
#1=app name, 2- db xml, 3-number of threads, 4-number of loops per thread, 5-log file prefix


testApp=$1
if [ -z ${testApp} ] ; then
  testApp=sednanativedbtest
fi


testdb=$2
if [ -z ${testdb} ] ; then
	testdb="testdb.xml"
fi

threads=$3
if [ -z ${threads} ] ; then
	threads=1
fi

numberOfLoops=$4
if [ -z ${numberOfLoops} ] ; then
	numberOfLoops=10
fi

 
logprefix=$5
if [ -z ${logprefix} ] ; then
   logprefix=test
fi
 
 
eval loopLimit=${threads}
curlog=test.log
cursumlog=summary.log

 

log=${threads}_${testApp}_${logprefix}.log
rm -rf ${log}
CMD_EXEC="time ./${testApp}/run_${testApp}.sh  ${threads} ${numberOfLoops} ${testdb} > ${log}"
printf "${CMD_EXEC}"	
eval $CMD_EXEC 
#grep EXEC ${log}
grep EXEC ${log} >> ${curlog}

grep SUMMARY ${log}
grep SUMMARY ${log} >> ${curlog}
grep SUMMARY ${log} >> ${cursumlog}

#cat   ${log} >>  ${curlog}

#printf "********* ${testApp} END  ${testdb}  T=${threads} L=${numberOfLoops} ${log} **********************\n"














