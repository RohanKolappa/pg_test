#!/bin/sh
if [ $# -ne 3 ]
then 
	echo "Syntax: startSimulator.sh dms-ip #streams image-dir"
	exit
fi

imagedir=$3
i=0
rport=7070
tport=7000
dmsip=$1

# Start the simulated Tx and Rx. Note the "-a 3" flag for
# the rx is there because older StreamingServers don't 
# automatically convert between audio-video packets and
# video-only packets.
while [ $i -lt $2 ]
do 
	i=`let ${i} + 1`
	echo "Executing rx -s $dmsip -p $rport -P $rport"
	./rx -s $dmsip -p $rport -P $rport -a 3
	echo "Executing tx -i $imagedir -p $tport"
	./tx -i $imagedir -p $tport
	rport=`let ${rport} + 2`
	tport=`let ${tport} +2`
done 

