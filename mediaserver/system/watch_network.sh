#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint() {
    logger $1
    echo $1
}

start() {
    logprint "starting watch_network.sh"
    /bin/touch /tmp/.watch_network
}

stop() {
    logprint "stopping watch_network.sh"
    /bin/rm -rf /tmp/.watch_network
}

watch() {
    if [ -f /tmp/.watch_network ]; then
        PHY0_STS=`cat /sys/class/net/eth0/phy0_status`
        PHY1_STS=`cat /sys/class/net/eth0/phy1_status`    
        if [ x${PHY0_STS} != "x1" ] ; then
	        logprint "Status: PHY0 is ${PHY0_STS}"
	        /V2O/bin/maint network_init -p 0
        fi
        if [ x${PHY1_STS} != "x1" ] ; then
	        logprint "Status: PHY1 is ${PHY1_STS}"
	        /V2O/bin/maint network_init -p 1
        fi
	        sleep 2
    fi
}

case "$1" in
    start)
        start
        ;;

    watch)
        watch
        ;;

    stop)
        stop
        ;;

    restart)
        stop
        start
        ;;

    *)
        echo "Usage: $0 [start|stop|watch|restart]"
        exit 1

esac
