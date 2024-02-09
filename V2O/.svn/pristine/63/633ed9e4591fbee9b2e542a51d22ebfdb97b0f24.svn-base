#!/bin/sh

while (true)
do
if [ -f /tmp/.rs232 ]
then
	sleep 10
else
	exec /sbin/getty -L ttyS0 $1 $2
fi
done
