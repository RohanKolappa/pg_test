#!/bin/sh


monitor_file=/tmp/streaming_server_restarts.log

logprint() {
    logger $1
    echo $1
}

start() {


    # If there's already file indicating Streaming Server is ready, remove it
    if [ -f /tmp/ssready ]; then  
        /bin/rm /tmp/ssready > /dev/null 2>&1
    fi

    # Make sure StreamingServer is not already running
    killall -0 StreamingServer > /dev/null 2>&1
    if [ "x$?" = "x0" ]; then
        logprint "StreamingServer already running... Kill it first."
        return
    fi

    # Start the Streaming Server
    logprint "Starting StreamingServer..."
    /home/onair/TV/bin/StreamingServer

    # Wait till StreamingServer is ready 
    count=0
    timeout=30
    while(true); do
        logprint "Waiting for StreamingServer to be ready. [${count}]" 
        if [ -f /tmp/ssready ]; then
            break
        fi
        count=`expr ${count} + 1`
        if [ $count -gt $timeout ]; then
            break
        fi
        sleep 1
    done

    # We begin monitoring only after the below file exists
    touch ${monitor_file}
}

stop() {

    logprint "Stopping StreamingServer..."

    # Stop monitoring
    cp ${monitor_file} ${monitor_file}.save > /dev/null 2>&1
    rm -f ${monitor_file}

    killall -0 StreamingServer > /dev/null 2>&1
    if [ "x$?" != "x0" ]; then
        logprint "StreamingServer not running."
        return
    fi

    # First do gracefull kill
    killall StreamingServer

    # Wait for it die else shoot using -9
    count=0
    while(true)
    do
        killall -0 StreamingServer > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            count=`expr ${count} + 1`
            if [ $count -gt 100 ]; then
                logprint "Timed out: Using kill -9 !!"
                killall -9 StreamingServer > /dev/null 2>&1
                break;
            fi
            /bin/usleep 100000
        else 
            break
        fi
    done
}


watch() {
    # do not monitor unless start() has been called
    if [ ! -f ${monitor_file} ]; then
        return
    fi

    # Restart streaming server if it is dead
    
    # Send signal 0 to StreamingServer. 
    # Exit status of kill command would be 0 if it is still alive
    killall -0 StreamingServer > /dev/null 2>&1
    if [ "x$?" != "x0" ];
    then
        tail -100 ${monitor_file} > ${monitor_file}.tmp
        echo  `date` >> ${monitor_file}.tmp
        /bin/dmesg | /bin/grep segfault | tail -1 >> ${monitor_file}.tmp
        /bin/mv ${monitor_file}.tmp ${monitor_file}
        logprint "Restarting StreamingServer"
        /home/onair/TV/bin/StreamingServer
        /bin/rm /tmp/killedstreaming
    fi

    # Restart streaming on GUI request 
    if [ -f /tmp/restartstreaming ]; 
    then
        tail -100 ${monitor_file} > ${monitor_file}.tmp
        echo "Restart Request: Killing Streaming Server" >> ${monitor_file}.tmp
        /bin/mv ${monitor_file}.tmp ${monitor_file}
        logprint "Restarting StreamingServer on User request"
        killall StreamingServer
        /bin/mv /tmp/restartstreaming /tmp/killedstreaming
        return
    fi

    # If killall failed use killall -9
    if [ -f /tmp/killedstreaming ]; 
    then
        tail -100 ${monitor_file} > ${monitor_file}.tmp
        echo "Restart Request: Killing -9 Streaming Server" >> ${monitor_file}.tmp
        /bin/mv ${monitor_file}.tmp ${monitor_file}
        logprint "Streaming Server stuck: Using killall -9"
        killall -9 StreamingServer
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
