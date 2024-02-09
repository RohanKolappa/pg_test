#These have to be defined
#------------------------
#TESTPREFIX="/tmp/miyer" if required
#SERVER_NAME="ManagementServer"
#THIS_SCRIPT_PATH="/etc/scripts/watchdog/watch_managementserver.sh"
#SERVER_CTL_FILE_PREFIX="ms"
#SERVER_STATUS_FILE_PREFIX="mserver"

#These have to be implemented
#------------------------
#start_implementation
#stop_implementation
#reset_implementation
#status_implementation

#if extending script has any extra commands e.g. HELP_ADDITONAL_CMDS="|export|import"
HELP_ADDITONAL_CMDS=""
if [ -z "$WATCHABLE" ]; then
    WATCHABLE="true"
fi

MANAGEMENT_SERVER_STOPPED="ManagementServer not started."
MANAGEMENT_SERVER_SCRIPT="/etc/scripts/watchdog/watch_managementserver.sh"
FILEPATH="${TESTPREFIX}/var/run"
STATUSPATH="${TESTPREFIX}/var/status"
LOCK_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.lck"
RUNNING_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.running"
CONFIGURED_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.conf"
MANUAL_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.manual"
ONAFTERSTOP_FILE="${FILEPATH}/${SERVER_CTL_FILE_PREFIX}.onafterstop"

STATUS_DOWN_FILE="${STATUSPATH}/${SERVER_STATUS_FILE_PREFIX}.down"
STATUS_UP_FILE="${STATUSPATH}/${SERVER_STATUS_FILE_PREFIX}.up"
FILE_FLAG_STATUS_DOWN_ACTION_NONE="${STATUSPATH}/${SERVER_STATUS_FILE_PREFIX}.down.donothing"

LOGS_DIR_PATH="/data/logs"
#spelling:do not preserve the file between reboots, move this file to /var/status/mserver.down.donothing.... 

logprint() {
    logger "$1"
    echo "$1"
}

logshow() {
    echo "$1"
}

check_lock() {
    # Check if we are already running
    if [ -f $LOCK_FILE ]; then
        # Make sure that the process that created 
        # this file is still around
        pid=`cat ${LOCK_FILE}`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            logshow "Busy: Another start/stop request still running..."
            return 1
        fi
        logshow "Warning: Previous start/stop attempt exited prematurely??"
        /bin/rm -f $LOCK_FILE
    fi

    # Now write current process's PID to the lock file.
    # Use noclobber to avoid a potential race condition
    if set -o noclobber; echo $$ > $LOCK_FILE ; then
        # Lock successfully acquired
        return 0
    fi

    logshow "Busy: Another start/stop request still running..."
    return 1
}

release_lock() {
    # Remove the lock file
    /bin/rm -f $LOCK_FILE
    return 0
}

start() {

    # Return if we are busy with another start/stop sequence
    check_lock || return 1

    # Return if we are already started
    if [ -f $RUNNING_FILE ] ; then
        logshow "${SERVER_NAME} already running!!"
        release_lock
        return 0
    fi

    touch $RUNNING_FILE

    /bin/rm -f $STATUS_UP_FILE

    # Warn if in auto mode
    if [ ! -f $MANUAL_FILE ]; then
        if [ "${WATCHABLE}" = "true" ]; then
            logshow "Warning: ${SERVER_NAME} seems to be in Auto mode. Suggest switching to Manual mode."
        fi
    fi

    logprint "${SERVER_NAME} Starting ..."

    # function to be implemented
    start_implementation
    ret=$?

    if [ "x$ret" != "x0" ]; then
        logprint "${SERVER_NAME} Failed to start."
        release_lock
        return $ret
    fi


    # Create Health file
    touch $STATUS_UP_FILE

    logprint "${SERVER_NAME} Started."

    release_lock
    return 0
}

stop() {

    # Return if we are busy with another start/stop sequence
    check_lock || return 1

    # Return if we are already stopped
    if [ ! -f $RUNNING_FILE ] ; then
        logshow "${SERVER_NAME} already stopped!!"
        release_lock
        return 0
    fi

    # Remove Health file
    /bin/rm -f $STATUS_UP_FILE

    # Warn if in auto mode
    if [ ! -f $MANUAL_FILE ]; then
        if [ "${WATCHABLE}" = "true" ]; then
            logshow "Warning: ${SERVER_NAME} seems to be in Auto mode. Suggest switching to Manual mode."
        fi
    fi
    
    logprint "${SERVER_NAME} Stopping..."

    # function to be implemented
    stop_implementation

    /bin/rm -f $RUNNING_FILE
    logprint "${SERVER_NAME} Stopped."

    release_lock

    if [ -f $ONAFTERSTOP_FILE ]; then
        logprint "${SERVER_NAME} Processing onafterstop ..."
        $THIS_SCRIPT_PATH on &
    fi
    
}

# Dump logs for debugging
dump_logs() {
     logprint "Dumping logs to $dirPath for debugging."

     if [ ! -d $LOGS_DIR_PATH ]; then
        logprint "INFO: Create log Dir ${LOGS_DIR_PATH}."
    	mkdir $uploadDir
     fi      
     
    /bin/rm -rf ${LOGS_DIR_PATH}/*

    dirName=`date +"%y-%m-%d-%H.%M.%S"`
    
    dirPath="${LOGS_DIR_PATH}/${dirName}"
    tarFileName="${LOGS_DIR_PATH}/logs.tar"
 
    /bin/mkdir -p $dirPath
    /bin/cp -f /var/log/messages* $dirPath
    /bin/cp -f /var/log/osgi/mrapp* $dirPath
    /bin/cp -rf /var/log/mscomponent $dirPath
    /bin/cp -rf /var/log/deviceapp $dirPath
    /bin/cp -rf /var/log/ipvscomponent $dirPath
    /bin/cp -rf /data/existdbserver/logs "$dirPath/existdbserver/"
    /bin/cp -rf /var/log/openfire $dirPath
    /bin/cp -rf /home/onair/openfire/logs "$dirPath/openfire/"
    /bin/cp -rf /var/log/osgi "$dirPath/"

    /bin/tar cvf ${tarFileName} -C ${LOGS_DIR_PATH} ${dirName}
    /bin/rm -rf ${dirPath}
}


# Request for reset to defaults
reset() {
    reset_implementation
}

watch() {

    watch_status 


    # Ensure that in "auto" mode, the service continues to run (or not run)
    # depending on current mode (controlled by "on" or "off" commands)

    # No need to do anything if we are in not in "auto" mode
    if [ -f $MANUAL_FILE ]; then
        return 0
    fi

    if [ -f $CONFIGURED_FILE ]; then
        if [ ! -f $RUNNING_FILE ]; then
            # Configured but currently not running
            $THIS_SCRIPT_PATH start &
        fi
    else
        if [ -f $RUNNING_FILE ]; then
            # Not configured but currently running
            $THIS_SCRIPT_PATH stop &
        fi
    fi

}

watch_status () {
    status_reason=`status`
    if [ "x$?" != "x0" ]; then
        if [ -f $RUNNING_FILE ]; then
            if [ ! -f $MANUAL_FILE ]; then
                #if this does not exists
                if [ ! -f  $FILE_FLAG_STATUS_DOWN_ACTION_NONE ]; then
                    logprint "Watch Dog Stopping ${SERVER_NAME}.}"
                    $THIS_SCRIPT_PATH stop &
                fi
            fi
        fi
    fi    
}

status() {

    if [ ! -f $RUNNING_FILE ]; then
        msg="APP_DOWN=${SERVER_NAME}\nRETURN_STATUS=1\nREASON=\"NOT_STARTED\""
        echo -e "$msg" > $STATUS_DOWN_FILE
        # Remove Health file
        /bin/rm -f $STATUS_UP_FILE

        logshow "${SERVER_NAME} not started."
        return 1
    fi

    
    # function to be implemented
    ret_status_reason=""
    status_implementation
    ret="$?"

    if [ "x$ret" != "x0" ]; then
        msg="APP_DOWN=${SERVER_NAME}\nRETURN_STATUS=${ret}\nREASON=\"$ret_status_reason\""
        echo -e "$msg" > $STATUS_DOWN_FILE
        # Remove Health file
        /bin/rm -f $STATUS_UP_FILE
        echo "DOWN"
        return $ret
    fi

    # Create Health file
    touch $STATUS_UP_FILE

    /bin/rm -f $STATUS_DOWN_FILE
    echo "OK"
    return 0
}

handle_command() {
    case "$1" in
        start)
            start
            ;;

        watch)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            watch
            ;;

        stop)
            stop
            ;;

        restart)
            stop
            start
            ;;

        reset)
            reset
            ;;

        status)
            status
            ;;
		
	  waitUntilStopped) 
		while (true)
		do
		    retVal=`$MANAGEMENT_SERVER_SCRIPT status`
		    ret="$?"
		    if [ "x$ret" = "x1" ]; then
			    # Make sure Management Server is stopped
      	 	    if [ "$retVal" = "$MANAGEMENT_SERVER_STOPPED" ]; then
			        logprint "${SERVER_NAME} stopped ..."
		      	  return 
			    fi		    
		    fi
		done
		;;

        on)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            /bin/touch  $CONFIGURED_FILE
            if [ -f $ONAFTERSTOP_FILE ]; then
                logprint "${SERVER_NAME} Clearing onafterstop ..."
                /bin/rm -f $ONAFTERSTOP_FILE
            fi
        ;;

        onafterstop)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            logprint "${SERVER_NAME} Setting onafterstop ..."
            /bin/touch  $ONAFTERSTOP_FILE
            ;;

        off)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            /bin/rm -f $CONFIGURED_FILE
            ;;

        manual)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            /bin/touch $MANUAL_FILE
            ;;

        auto)
            if [ "${WATCHABLE}" != "true" ]; then
                return 0
            fi
            /bin/rm -f $MANUAL_FILE
            ;;

        *)
        if [ "${WATCHABLE}" = "true" ]; then
            echo "Usage: $0 [start|stop|on|off|auto|manual|watch|restart|reset|status${HELP_ADDITONAL_CMDS}]"
        else
            echo "Usage: $0 [start|stop|restart|reset|status${HELP_ADDITONAL_CMDS}]"
        fi
        exit 1
    esac
}
