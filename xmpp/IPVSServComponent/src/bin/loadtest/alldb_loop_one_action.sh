#!/bin/sh

#help 
#1-db xml, 2-number of threads, 3-loop interval, 4-number of loops per thread. 

TESTDBXML=$1
if [ -z ${TESTDBXML} ] ; then
	TESTDBXML="testdb.xml"
fi

maxThreads=$2
if [ -z ${maxThreads} ] ; then
	maxThreads=30
fi

skipThreadsInterval=$3
if [ -z ${skipThreadsInterval} ] ; then
	skipThreadsInterval=1
fi

numberOfLoops=$4
if [ -z ${numberOfLoops} ] ; then
	numberOfLoops=110
fi


rm -rf *.log

echo "test TESTDBXML=${TESTDBXML} maxThreads=${maxThreads} skipThreadsInterval=${skipThreadsInterval} numberOfLoops=${numberOfLoops}"

#loopLimit=`expr $maxThreads + 1`
#loopInterval=`expr $skipThreadsInterval + 1`

eval loopLimit=$maxThreads
eval loopInterval=$skipThreadsInterval

echo "test TESTDBXML=${TESTDBXML} maxThreads=${loopLimit} skipThreadsInterval=${loopInterval} numberOfLoops=${numberOfLoops}"


#count=0								 
#while [ $count -lt $loopLimit ]						 
#do								 
#    count=`expr $count + $loopInterval`					 
#    echo "${count}"				 
#done	

for ((  i = 1 ;  i <= ${loopLimit};  i=`expr $i + $loopInterval` ))
do
	echo "create number of threads=${i} \n "
	
	printf "\n existdbtest threads=${i} numberOfLoops=${numberOfLoops} test=${TESTDBXML}  log=exist${i}.log"
	time ./existdbtest/run_existdbtest.sh  ${i} ${numberOfLoops} ${TESTDBXML}  > exist${i}.log

	printf "\n sednanativedbtest threads=${i} numberOfLoops=${numberOfLoops} test=${TESTDBXML}  log=sednanative${i}.log"
	time ./sednanativedbtest/run_sednanativedbtest.sh  ${i} ${numberOfLoops} ${TESTDBXML}  > sednanative${i}.log

	printf "\n sednaxmldbtest threads=${i} numberOfLoops=${numberOfLoops} test=${TESTDBXML}  log=sednaxmldb${i}.log"
	time ./sednaxmldbtest/run_sednaxmldbtest.sh  ${i} ${numberOfLoops} ${TESTDBXML} > sednaxmldb${i}.log

done
