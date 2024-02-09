#!/bin/sh
while(true)
do
if [ -f /tmp/.rs232 -o -f /etc/config/no_serial_console ];
then
	sleep 10
else
	exec /sbin/getty $@
fi
done

