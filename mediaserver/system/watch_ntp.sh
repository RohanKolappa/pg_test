#!/bin/sh

NTPD=/home/onair/bin/ntpd
NTPQ=/home/onair/bin/ntpq
NTPDCLIENTCONF=/home/onair/ntp.conf
NTPDSERVERCONF=/home/onair/ntp_server.conf
NTPDSTATUS=/tmp/ntp_status.txt
NTPDATESTATUS=/tmp/ntp_date.txt
NTPDOFFSET=/tmp/ntp_offset.txt

logprint() {
    logger $1
    echo $1
}

start() {

    # Do not start anything unless NTP conf file exists
    if [ ! -f $NTPDCLIENTCONF -a ! -f $NTPDSERVERCONF ]; then
        logprint "NTP service not configured."
        return
    fi

    # Find out if we are server mode or client mode
    if [  -f $NTPDSERVERCONF ]; then
       NTPDCONF=$NTPDSERVERCONF
    else
       NTPDCONF=$NTPDCLIENTCONF
    fi

    # Make sure StreamingServer is not running
    killall -0 StreamingServer > /dev/null 2>&1
    if [ "x$?" = "x0" ]; then
        logprint "Cannot start NTP service... Streaming Server is running."
        return
    fi

    # Make sure ntp daemon is not running
    killall -0 ntpd > /dev/null 2>&1
    if [ "x$?" = "x0" ]; then
        logprint "Cannot start NTP service. Stop the service first."
        return
    fi

    # Ready to start the service
    logprint "Starting NTP service..."

    # Create the status file. If any attempt is made to start the service
    # this file should always exist
    touch $NTPDSTATUS

    # If NTP server ip address has been configured, 
    # first, sync up to the current time
    grep NTPSERVERIP $NTPDCONF > /dev/null 2>&1
    if [ "x$?" != "x0" ]; then
        logprint "Synchronizing the current time"
        rm -f $NTPDATESTATUS
        $NTPD -g -q -c $NTPDCONF > $NTPDATESTATUS &
        
        # Wait for the ntpd daemon started above to quit
        count=0
        sleep 1
        while(true); do
            killall -0 ntpd > /dev/null 2>&1
            if [ "x$?" != "x0" ]; then
                break
            fi
            count=`expr ${count} + 1`
            if [ $count -gt 30 ]; then
               logprint "Killing stuck ntpd"
               killall ntpd > /dev/null 2>&1
               break
            fi
            sleep 1
        done

        # Check if the inital date sync succeeded
        grep "ntpd: time" $NTPDATESTATUS > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            /sbin/hwclock -u -w # set the hardware to the latest time retrieved
            logprint "Initial Time Sync Suceeded."
            echo "Initial Time Sync Suceeded." > $NTPDSTATUS
        else
            logprint "Error: Initial Time Sync Failed."
            echo "Error: Initial Time Sync Failed." > $NTPDSTATUS
        fi
    fi

    $NTPD -x -c $NTPDCONF # start the daemon
    logprint "NTP service started."
    
}

stop() {
    
    logprint "Stopping NTP service..."

    # Just kill the daemon
    killall ntpd > /dev/null 2>&1
    
    # And remove the status file
    /bin/rm -f $NTPDSTATUS

}

watch() {
    # No need to monitor the daemon if service was never started
    if [ ! -f $NTPDSTATUS ]; then
        return
    fi

    # The daemon better be running
    killall -0 ntpd > /dev/null 2>&1
    if [ "x$?" != "x0" ]; then
        # ntpd exited for some reason
        echo "Failed: Panic threshold exceeded." > $NTPDSTATUS
        return
    fi

    # Run the ntpq monitor command to retrieve the status
    syncstatus=`$NTPQ -p -n | tail -1`
    peerstate=`echo $syncstatus | cut -c 1`
    if [ "x${peerstate}" = "x*" ]; then
        echo "Ok" > $NTPDSTATUS
    else
        echo "Error: No sync." > $NTPDSTATUS
    fi
    peeroffset=`echo $syncstatus |  sed -r -e 's/[ \t]+/ /g' | cut -f 9 -d ' '`
    echo ${peeroffset} > $NTPDOFFSET

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
