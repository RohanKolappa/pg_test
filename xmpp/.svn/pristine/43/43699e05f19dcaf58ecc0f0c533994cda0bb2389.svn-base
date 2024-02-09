#!/bin/sh
 

testdb=$1
if [ -z ${testdb} ] ; then
	testdb="sednanativedbtest"
fi

STATUS_FILE=/tmp/shutdown_dms_171.status
while (true); do 
	err=0
	count=0
	timout=600
    #timout=`expr ${timout} * 5`
	while(true); do
		if [ ! -f $STATUS_FILE ]; then
            break
		fi
		echo "Waiting to start Sedna on 10.1.5.171. [${count}/${timout}]"
		sleep 5
		count=`expr ${count} + 5`
	done

	./db_loop_all_action.sh ${testdb}
	
	sleep 5 
	
	echo "sleep ${testdb}" 

done
 
 