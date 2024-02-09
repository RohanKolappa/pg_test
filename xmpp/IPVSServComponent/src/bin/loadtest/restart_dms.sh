#!/bin/sh
# Run test forever till first error
#corr_dir=`pwd`
cd /home/pravin/tcl/automation2

tool="db_scripts" 
test_script="restart_dms"

DMS="10.1.5.171"
ps1IP="192.168.1.208"
STATUS_FILE="/tmp/shutdown_dms_171.status"

# runtest --tool db_scripts restart_dms.exp
count=1
while (true); do
 
    echo "Create shutdown status file..$STATUS_FILE"
	echo "shuttingdown " > $STATUS_FILE
    
    runtest --tool "${tool}" "${test_script}.exp"

	echo "Sleep 30 seconds- to remove shutdown status file $STATUS_FILE .."
    sleep 80

	echo "remove shutdown status file..$STATUS_FILE"
	rm -f $STATUS_FILE

	echo "Sleep 1 hour-  to Power OFF ${DMS} .."
    sleep 300
 
    # Run the test
    echo "RUN#  $count for script \"${test_script}.exp\" in tool \"${tool}\"..."

    count=`expr $count + 1`;
done


#coru_dir $coru_dir



