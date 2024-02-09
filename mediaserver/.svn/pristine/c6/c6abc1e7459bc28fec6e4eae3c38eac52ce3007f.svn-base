#!/bin/sh

source "${SCRIPT_PREFIX}/ctlbase.sh"

PID_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.pid"

start_implementation() {

    if [ -f $PID_FILE ] ; then
        pid=`cat $PID_FILE`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            logprint "${SERVER_NAME} already running... Stop it first."
            return 0
        fi
    fi

    start_command

    ret=$?

    #echo "PID=${PID}"

    if [ "x$?" = "x0" ]; then
        echo $PID > $PID_FILE
    fi

    return 0
}

stop_implementation() {

    if [ -f $PID_FILE ] ; then
        pid=`cat $PID_FILE`
    else
       logprint "${SERVER_NAME} not running."
       return
    fi

    # First do gracefull kill
    #logprint "${SERVER_NAME} graceful kill."
    kill $pid  > /dev/null 2>&1

    # Wait for it die else shoot using -9
    count=0
    while(true)
    do
    	#logprint "${SERVER_NAME} forceful kill."
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

    #logprint "${SERVER_NAME} killed."

    rm -rf $PID_FILE

    return 0
}

reset_implementation() {
  echo "Cluster reset"
}

status_implementation() {

    if [ -f $PID_FILE ] ; then
        pid=`cat $PID_FILE`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            # still running normally
            return 0
        fi
    fi

    ret_status_reason="${SERVER_NAME} not running"
  
    return 1
}

