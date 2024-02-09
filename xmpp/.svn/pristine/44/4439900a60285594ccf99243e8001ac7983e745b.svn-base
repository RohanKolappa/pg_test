#!/bin/sh


#help 
#1- db xml, 2-number of threads, 3-number of loops per thread, 4-log file prefix


testdb=$1
if [ -z ${testdb} ] ; then
	testdb="testdb.xml"
fi

threads=$2
if [ -z ${threads} ] ; then
	threads=30
fi

numberOfLoops=$3
if [ -z ${numberOfLoops} ] ; then
	numberOfLoops=100
fi


logprefix=$4
if [ -z ${logprefix} ] ; then
	logprefix=test
fi



./run_test_db.sh  sednaxmldbtest ${testdb} ${threads} ${numberOfLoops} ${logprefix}
./run_test_db.sh  existdbtest ${testdb} ${threads} ${numberOfLoops} ${logprefix}
./run_test_db.sh  sednanativedbtest ${testdb} ${threads} ${numberOfLoops} ${logprefix}



