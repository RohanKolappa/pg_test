#!/bin/sh

dateSet()
{
	/bin/sh /tmp/date.sh 2>&1 >> /dev/NULL
	/sbin/hwclock -w
}

stop() {
	echo "Shutting down"
	echo "stopSent" > /tmp/status.led

	if [ -f /etc/pre_shutdown.sh ] ; then
		/etc/pre_shutdown.sh
	fi 
	
	/bin/echo "Terminating Streaming Server"
    /bin/sh /etc/scripts/watchdog/watch_streamingserver.sh stop

	/bin/echo "Terminating NTP"
    /bin/sh /etc/scripts/watchdog/watch_ntp.sh stop

	/bin/echo "Terminating Management Server"
    /bin/sh /etc/scripts/watchdog/watch_managementserver.sh manual
    /bin/sh /etc/scripts/watchdog/watch_managementserver.sh stop

	/bin/echo "Terminating Device app"
    /bin/sh /etc/scripts/watchdog/watch_deviceapp.sh stop

	/bin/echo "Terminating Linux-HA"
    /bin/sh /etc/scripts/watchdog/watch_linuxha.sh stop

    if [ -f /etc/config/arm_system ] ; then 
      /bin/echo "Stopping network watchdog"
      /bin/sh /etc/scripts/watchdog/watch_network.sh stop
    fi
	sleep 2

    if [ -f /etc/config/sw_profile_v2dxp200.txt ] ; then 
      /bin/echo "Stopping I2C watchdog"
      /bin/sh /etc/scripts/watchdog/watch_i2c.sh stop
    fi
	sleep 2

    if [ ! -f /etc/config/arm_system ]; then
	    /bin/echo "Releasing shared memory resources"
	    for i in `ipcs |grep 0x |grep -v grep | cut -c12-21`
	    do
		    ipcrm shm $i
	    done
    fi


    # Sync hardware clock to the latest time
    /sbin/hwclock -u -w
        
	/bin/echo "Unmounting data partitions"
    sync
    sync
	/bin/umount /data1
	/bin/umount /data2

	if [ -f /etc/post_shutdown.sh ] ; then
		/etc/post_shutdown.sh
	fi 


	echo "stopComplete" > /tmp/status.led
	sleep 2
}


while (true)
do

	if [ !  -f /tmp/down ] ;
	then
		if [ -f /tmp/shutdown ];
		then
                        /bin/echo -n "Device Shutdown: " >> /data/uptime.log
                        /bin/date >> /data/uptime.log
                        /usr/bin/uptime >> /data/uptime.log
			stop
			touch /tmp/down
			/bin/busybox poweroff
		fi
	fi

	if [ -f /tmp/reset ];
	then
		/bin/rm /tmp/reset
		echo "resetSent" > /tmp/status.led
                /bin/echo -n "Device Reset: " >> /data/uptime.log
                /bin/date >> /data/uptime.log
                /usr/bin/uptime >> /data/uptime.log
		stop
		/bin/busybox reboot
                exit
	fi

	if [ -f /tmp/date.sh ];
	then
                /bin/echo -n "Device DateSet: " >> /data/uptime.log
                /bin/date >> /data/uptime.log
                /usr/bin/uptime >> /data/uptime.log
		stop
		dateSet
		/bin/rm /tmp/date.sh
		/bin/busybox reboot
                exit
	fi	
   
	#check if linux ha exists, and if true run it
	hascriptfile="/etc/scripts/watchdog/watch_linuxha.sh"
	haconfFile="/var/run/linuxha.conf"
	ms_scriptfile="/etc/scripts/watchdog/watch_managementserver.sh"

	# Run all watchdog scripts found in /etc/scripts/watchdog/ directory except management server and linuxha
	watchdog_scripts=`/bin/ls /etc/scripts/watchdog/*.sh`
	for scriptfile in $watchdog_scripts	
	do
	#echo "$scriptfile"
	if [ -x $scriptfile ] # if file is executable
	then
		if [ "${scriptfile}" = "${hascriptfile}" ] || [ "${scriptfile}" = "${ms_scriptfile}" ] ; then
		     continue;
		fi
		/bin/sh ${scriptfile} watch > /dev/null 2>&1

	fi
	done


	if [ -f "${hascriptfile}" ]; then
	  /bin/sh ${hascriptfile} watch > /dev/null 2>&1
	fi

	watch_ms="/var/run/watchms"

	#start watch on management server only in case of primary server or HA is disabled.
	if [ ! -f "${haconfFile}" ] || [ -f "${watch_ms}" ] ; then
	 /bin/sh ${ms_scriptfile} watch > /dev/null 2>&1
	fi

	sleep 2
done
