#!/bin/sh

#help 
#1=app name, 2- db xml, 3-number of threads, 4-number of loops per thread, 5-log file prefix


testApp=$1
if [ -z ${testApp} ] ; then
  testApp=sednanativedbtest
fi


testdb=$1
if [ -z ${testdb} ] ; then
	testdb="testdb.xml"
fi

threads=$2
if [ -z ${threads} ] ; then
	threads=1
fi

numberOfLoops=$3
if [ -z ${numberOfLoops} ] ; then
	numberOfLoops=10
fi

 
logprefix=$4
if [ -z ${logprefix} ] ; then
	logprefix=test
fi
 
 
eval loopLimit=${threads}

curlog=test.log
rm -rf ${curlog}

 

log=${logprefix}${testApp}${threads}.log
rm -rf ${log}
CMD_EXEC="time ./${testApp}/run_${testApp}.sh  ${threads} ${numberOfLoops} ${testdb} > ${log}"

printf "\n\n\n********* ${testApp} START  ${testdb}  T=${threads} L=${numberOfLoops} ${curlog} **********************"
printf "\n${CMD_EXEC} \n   "	

eval $CMD_EXEC 
mv -f ${curlog} ${log}
grep SUMMARY ${log}
printf "********* ${testApp} END  ${testdb}  T=${threads} L=${numberOfLoops} ${log} **********************\n"














