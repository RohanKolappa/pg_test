#!/bin/sh

# Source common library
. /etc/scripts/common.rc

monitor_file=/tmp/deviceapp_restarts.log
agent_status_file=/tmp/status
deviceapp_disabled="/tmp/deviceapp.disabled"
avahi_di_service=/etc/avahi/services/di.service

DEVICEAPP_PIDFILE="/var/run/deviceapp.pid"
DEVICE_APP_NAME="DeviceApp"
STATUS_INITIALIZING_FILE="/var/status/${DEVICE_APP_NAME}.java.initializing"
STANDALONE=0

SERIAL_NUMBER=`cat /etc/config/device_serial_number`


#DMSApp should run appserver agent ? disableAppServerAgent=true/false
#disableAppServerAgent=true ... AppServerAgent will be not started in DMSApp
disableAppServerAgent=false
MSCOMPONENT_HOME=/home/onair/mscomponent
if [ -f ${MSCOMPONENT_HOME}/bin/_disable_mscomponent.rc ] ; then
	. ${MSCOMPONENT_HOME}/bin/_disable_mscomponent.rc
fi
register_di_service() {
	local agent=`xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml|sed -e 's/-Device//'`
	local mac_address=`ifconfig eth0 | grep HWaddr | sed -e 's/.*HWaddr \(.*\)/\1/'`
	local hash_of_mac_address=`echo ${mac_address}| sed -e's/://g'`

	sed -e "s/TypeAndHashOfMac/ ${agent} ${SERIAL_NUMBER}/" \
		/home/onair/templates/di.service |\
	sed -e "s/MACADDRESS/${mac_address}/" > /etc/avahi/services/di.service
}

unregister_di_service() {
    rm -f /etc/avahi/services/di.service
}
update_di_service() {
	local agent_error;
	local di_unregistered;
	xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml|grep -e 'V2D-XP' -e 'V2D-Tx' -e 'V2D-Rx'
	local isXPDevice = $?
	if [ -f ${agent_status_file} ] ; then
		if [$isXPDevice = "1"] ; then
			grep -c "state='1'" ${agent_status_file}
			agent_error=$?
		else 
			agent_error=0
		fi
	else
		agent_error=1
	fi
# Check if the di is unregistered
	grep -c "Registered=Yes" /etc/avahi/services/di.service
	di_unregistered=$?
	if [ $agent_error != $di_unregistered ]; then
		if [ $agent_error = "1" ]; then
			sed -i -e "s/Registered=Yes/Registered=No/" /etc/avahi/services/di.service
		else
			sed -i -e "s/Registered=No/Registered=Yes/" /etc/avahi/services/di.service
		fi
	fi
	
}

update_xpidi_service() {
    if [ -f ${agent_status_file} ] ; then
        if [ ${agent_status_file} -nt ${avahi_di_service} ]; then
            sed -i -e "s/Registered=No/Registered=Yes/" ${avahi_di_service}
            sleep 2
            touch ${monitor_file}
        fi
    else
        if [  ${monitor_file} -nt ${avahi_di_service} ]; then
            sed -i -e "s/Registered=Yes/Registered=No/" ${avahi_di_service}
        fi
    fi

}


hup() {
    if [ -f $DEVICEAPP_PIDFILE ] ; then
      if [ -f /etc/config/LoginRequest.xml ] ; then
        /bin/touch /etc/config/LoginRequest.xml
      fi
    fi
}

start() {


    # Make sure Device App is not already running
    if [ -f $DEVICEAPP_PIDFILE ] ; then
        pid=`cat $DEVICEAPP_PIDFILE`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            logprint "DeviceApp already running... Stop it first."
            return
        fi
    fi
    
    if [ -f /etc/config/arm_system ] ; then
    	/bin/rm -rf /etc/agent/db/streamcache/*
    	/bin/rm -rf /var/www/login.xml
    fi

	register_di_service

    # Refuse to start in standalone mode
    if [ $STANDALONE = 1 ] ; then
        logprint "Cannot start DeviceApp in standalone mode."
        return
    fi

    # Refuse to start when HA is configured
    # This makes sure deviceapp is not started when HA is enabled. 
    # The standby MS doesn't login as a regular device with this fix.
    if [ -f ${deviceapp_disabled} ] ; then
        logprint "Cannot start DeviceApp when HA is configured."
        return
    fi	
	TOTALSYSMEM=0
    # Determine Device App name
    AGENT=`get_device_interface_agent_name`
    # Determine memory to allocate for device app
	JAVA_OPTS=
	if [ ! -f /etc/config/arm_system ] ; then
	    TOTALSYSMEM=`cat /proc/meminfo | grep MemTotal |  sed -r  's/^[^0-9]*([0-9]+).*/\1/'`
	    if [ $TOTALSYSMEM -gt 7000000 ] ; then
    		JAVA_OPTS="-Xmx256m"
    	else
        	JAVA_OPTS="-Xmx128m"
        fi
	fi
    
    if [ -f /etc/config/sw_profile_v2d.txt ] ; then
        JAVA_OPTS="-Xmx8m"
    elif [ -f /etc/config/sw_profile_v2dxp200.txt ] ; then
        JAVA_OPTS="-Xmx16m"    
    fi
	GC_OPTS="-XX:+UseG1GC -XX:MinHeapFreeRatio=10 -XX:MaxHeapFreeRatio=25"
    # Change directory to where we want to create the nohop.out file
    current_dir=`pwd`
    cd /var/log/deviceapp

    logprint "Starting DeviceApp..."

	# Delete the status file used for the management server connection
	if [ -f ${agent_status_file} ] ; then
	    /bin/rm ${agent_status_file}
	fi
	case "${AGENT}" in
		dms|ms)
			nohup java ${JAVA_OPTS} ${GC_OPTS} -Dlog4j.configuration=file:///home/onair/modules/devicevng/public/log4j.xml -XX:OnOutOfMemoryError="kill -9 %p" -classpath /home/onair/modules/devicevng/public/${AGENT}.jar  com.barco.device.app.DeviceApp login /etc/config/LoginRequest.xml loginstatus /tmp/status config /etc/config/Device.xml status /tmp/Device.xml &
			;;
		v2dxp|v2drx|v2dtx)			
			nohup java ${JAVA_OPTS} -Dlog4j.configuration=file:///home/onair/modules/devicevng/public/log4j.xml -XX:OnOutOfMemoryError="kill -9 %p" -classpath /home/onair/modules/devicevng/public/${AGENT}.jar  com.barco.device.app.DeviceApp login /etc/config/LoginRequest.xml loginstatus /tmp/status config /etc/config/Device.xml status /tmp/Device.xml &
			;;
		v2dxpi)
			/usr/bin/agent /etc/config/LoginRequest.xml /var/www/login.xml &
			;;
		*)
			AGENT=`get_agent_app_name`
		nohup /usr/bin/java ${JAVA_OPTS} ${GC_OPTS} -XX:OnOutOfMemoryError="kill -9 %p" -cp /home/onair/modules/mediaroom/public/mediaroom.jar com.ipvs.device.${AGENT} login /etc/config/LoginRequest.xml servicedomain /etc/config/SetServiceDomainRequest.xml config /etc/config/Device.xml disableAppServerAgent ${disableAppServerAgent} &
	esac
    

    PID=$!

    if [ "x$?" = "x0" ]; then
        echo $PID > $DEVICEAPP_PIDFILE
    fi

	if [ x"${AGENT}" != "xv2dxpi" ]; then

    # create STATUS_INITIALIZING_FILE , DeviceApp will remove it on initialization complete.    
    echo "pid=${PID}" > $STATUS_INITIALIZING_FILE
    local err=0
    count=0
    timout=120
    while(true); do
	 count=`expr ${count} + 1`
    	 if [ ! -f $STATUS_INITIALIZING_FILE ]; then
	 	break
	 fi
	 if [ $count -gt $timout ]; then
		err=1
		break
	 fi        
	 logprint "Waiting to start ${DEVICE_APP_NAME}. [${count}/${timout}]"
	 sleep 1
    done
	  
    if [ $err = "1" ] ; then
	 cd $current_dir
	 logprint "ERROR: Could not start ${DEVICE_APP_NAME}."
	 return 1
    fi
	fi
    logprint "DeviceAppStarted"
    
    cd $current_dir

    # We begin monitoring only after the below file exists
    touch ${monitor_file}
}

stop() {


    # Stop monitoring
    rm -f ${monitor_file}

    unregister_di_service

    if [ -f $DEVICEAPP_PIDFILE ] ; then
        pid=`cat $DEVICEAPP_PIDFILE`
    else
       logprint "DeviceApp not running."
       return
    fi


    logprint "Stopping DeviceApp..."

    # First do gracefull kill
    kill $pid  > /dev/null 2>&1

    # Wait for it die else shoot using -9
    count=0
    while(true)
    do
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            count=`expr ${count} + 1`
            if [ $count -gt 100 ]; then
                logprint "Timed out: Using kill -9 !!"
                kill -9 $pid > /dev/null 2>&1
                break;
            fi
            /bin/usleep 100000
        else 
            break
        fi
    done
    
    # Delete the status file used for the management server connection
    /bin/rm -f ${agent_status_file}

    # Finally remove the PID file
    rm -f $DEVICEAPP_PIDFILE

}


watch() {
    # do not monitor unless start() has been called
    if [ ! -f ${monitor_file} ]; then
        return
    fi

    if [ -f /etc/config/arm_system ] ; then
       update_xpidi_service
    else			
       update_di_service
    fi			 

    # Is DeviceApp running?
    if [ -f $DEVICEAPP_PIDFILE ] ; then
        pid=`cat $DEVICEAPP_PIDFILE`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            # DeviceApp still running normally
            return
        fi
    fi

    # Device APP is not running for some reason. Start it again
    tail -100 ${monitor_file} > ${monitor_file}.tmp
    echo  `date` >> ${monitor_file}.tmp
    /bin/mv ${monitor_file}.tmp ${monitor_file}
    logprint "Restarting DeviceApp"

    # Restart Streaming server too
    # TODO: remove this hack when DeviceApp handles restarts cleanly
    if [ -f /etc/scripts/watchdog/watch_streamingserver.sh ]; then
        /etc/scripts/watchdog/watch_streamingserver.sh stop
        /bin/rm /home/onair/appcontrol
        /bin/rm /home/onair/recovery
        /etc/scripts/watchdog/watch_streamingserver.sh start
    fi

    # Start the DeviceApp
    start
    
}

# Are we in standalone mode on V2D?
if [ -f /etc/config/sw_profile_v2d.txt ] ; then
    grep STANDALONE /V2O/config/standaloneoperation.txt | grep -i "yes" > /dev/null 2>&1
    if [ x$? = "x0" ] ; then
        STANDALONE=1
    fi
fi

if [ -f /etc/config/sw_profile_v2dxp200.txt ] ; then
    grep STANDALONE /V2O/config/standaloneoperation.txt | grep -i "yes" > /dev/null 2>&1
    if [ x$? = "x0" ] ; then
        STANDALONE=1
    fi
fi
case "$1" in
  start)
        start
        ;;

  forcestart)
        STANDALONE=0
        start
        ;;

  watch)
        watch
        ;;

  enabled)
	/bin/rm -f ${deviceapp_disabled}
	;;
  
  disabled)
	touch ${deviceapp_disabled}	
	;;  

  stop)
        stop
        ;;

  restart)
        stop
        start
        ;;

  hup)
        hup
        ;;

  *)
        echo "Usage: $0 [start|stop|enabled|disabled|watch|restart|hup]"
        exit 1

esac
