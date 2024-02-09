#!/bin/sh

# Source common library
. /etc/scripts/common.rc

STOPPED_FILE="/var/run/nas.stopped"
WATCHED_FILE="/var/run/nas.watched"
LOGGERTAG=nasctl
NASMOUNTROOT=/data1/DSS/data/portal/site/NAS
#NASMOUNTOPTIONS=hard,intr,nolock,nfsvers=3
NASMOUNTOPTIONS=soft,nolock,nfsvers=3
NASMOUNTOPTIONS=soft,nolock,tcp,nfsvers=3
NASMOUNTOPTIONS=soft,nolock,udp,rsize=8192,wsize=8192,nfsvers=3
THIS_SCRIPT_PATH="/etc/scripts/watchdog/watch_nas.sh"
NASLOCKPREFIX="/var/run/nas_check_"
NFSLOGPREFIX="/var/run/nfs.log."

change_state() {
    #logprint "Change STATE to ${1} in ${2}"
    /bin/sed -i "s/STATE=.*/STATE=${1}/" ${2}
}

change_param() {
    #logprint "Changed ${1} to ${2} in ${3}"
    /bin/sed -i "s#${1}=.*#${1}=${2}#" ${3}
}

mount_datastore() {

    nasid=$1

    local config_file=${STATUS_DIR}/nas_${nasid}.conf
    local status_file=${STATUS_DIR}/nas_${nasid}.status

    . ${config_file}
    /bin/rm ${config_file}

    NASMOUNTPOINT=${NASMOUNTROOT}/${nasid}
    /bin/mkdir -p ${NASMOUNTPOINT}
    /bin/rm -f ${NFSLOGPREFIX}${nasid}


    # If something is already mounted at this mount point, unmount it first
    /bin/mount | grep "type nfs " | grep ${NASMOUNTPOINT} > /dev/null 2>&1
    if [ x$? = "x0" ]; then
        logerror "Some directory is already mounted at ${NASMOUNTPOINT}" 
        /bin/umount ${NASMOUNTPOINT} > ${NFSLOGPREFIX}${nasid} 2>&1
    fi

    # Mount the NFS directory

    if [ x${READONLY} = "x1" ] ; then
        NASMOUNTOPTIONS=${NASMOUNTOPTIONS},ro
        change_param READONLY 1 ${status_file}
    fi
    
    # TODO: Remove this line once debugged
    #echo /bin/mount -t nfs -o ${NASMOUNTOPTIONS} ${NFS_SERVER}:${REMOTE_DIR} ${NASMOUNTPOINT} > /tmp/nas_command

    
    /bin/mount -t nfs -o ${NASMOUNTOPTIONS} ${NFS_SERVER}:${REMOTE_DIR} ${NASMOUNTPOINT} > ${NFSLOGPREFIX}${nasid} 2>&1
    local ec=$?

    # Did we really succeed in mounting??
    /bin/mount | grep ${NFS_SERVER}:${REMOTE_DIR} > /dev/null 2>&1
    if [ x$? != "x0" -o x$ec != "x0" ]; then
        cat ${NFSLOGPREFIX}${nasid} | logger
        logerror "Failed to mount : ${NFS_SERVER}:${REMOTE_DIR} at ${NASMOUNTPOINT}" 
        change_state "retry" ${status_file}
        change_param STATUS "Error" ${status_file}
        local info=`head -1 ${NFSLOGPREFIX}${nasid}`
        change_param INFO "\"${info}\"" ${status_file}
        /V2O/bin/send_notification -s NAS -t Error Index ${nasid} ID ${NAME} ACTION Mount
        return 1
    fi

    # Sucessfully mounted
    change_state "mounted" ${status_file}

    # Sanity check: In readwrite mode, we should be able to create a
    # marker file. In readonly mode we should be able to detect a file
    # we  created during a previous mount.
    if [ x${READONLY} = "x0" ] ; then
        /bin/date > ${NASMOUNTPOINT}/mount.date
        if [ x$? != "x0" ]; then
            logerror "Could not write to ${NFS_SERVER}:${REMOTE_DIR} at ${NASMOUNTPOINT}"
            change_param STATUS "WriteFail" ${status_file}
            change_param INFO "\"Could not write to drive\"" ${status_file}
            return 1
        fi    
    else 
        /usr/bin/wc ${NASMOUNTPOINT}/mount.date
        if [ x$? != "x0" ]; then
            logerror "Could not recognize ${NFS_SERVER}:${REMOTE_DIR} at ${NASMOUNTPOINT} as a data store"
            change_param STATUS "ReadFail" ${status_file}
            change_param INFO "\"Could not recognize our drive\"" ${status_file}
            return 1
        fi    
    fi

    /bin/df ${NASMOUNTPOINT} > ${STATUS_DIR}/nas_${nasid}.df.tmp
    /bin/mv ${STATUS_DIR}/nas_${nasid}.df.tmp ${STATUS_DIR}/nas_${nasid}.df
    
    change_param STATUS "Ok" ${status_file}
    change_param INFO "" ${status_file}
    logprint "Mounted ${NFS_SERVER}:${REMOTE_DIR} at ${NASMOUNTPOINT}"
    /V2O/bin/send_notification -s NAS -t Mount Index ${nasid} ID ${NAME}

}

unmount_datastore() {

    nasid=$1
    local config_file=${STATUS_DIR}/nas_${nasid}.conf
    local status_file=${STATUS_DIR}/nas_${nasid}.status
    . ${config_file}
    /bin/rm ${config_file}

    NASMOUNTPOINT=${NASMOUNTROOT}/${nasid}

    # Have we already mounted the NFS volume??
    /bin/mount | grep "type nfs " | grep ${NASMOUNTPOINT} > /dev/null 2>&1
    if [ x$? != "x0" ]; then
        logprint "Nothing mounted at ${NASMOUNTPOINT}" 
        change_state "unmounted" ${status_file}
        change_param STATUS "Disabled" ${status_file}
        change_param INFO "" ${status_file}
        return 0
    fi
    /bin/rm -f ${NFSLOGPREFIX}${nasid}
    
    # Unmount
    /bin/umount ${NASMOUNTPOINT} > ${NFSLOGPREFIX}${nasid} 2>&1
    local ec=$?
    
    # Did we really unmount successfully??
    /bin/mount | grep "type nfs " | grep ${NASMOUNTPOINT} > /dev/null 2>&1
    if [ x$? = "x0" -o x$ec != "x0" ]; then
        cat ${NFSLOGPREFIX}${nasid} | logger
        logerror "Failed to unmount ${NASMOUNTPOINT}" 
        change_state "retryunmount" ${status_file}
        change_param STATUS "Error" ${status_file}
        change_param INFO "Failed to unmount" ${status_file}
        /V2O/bin/send_notification -s NAS -t Error Index ${nasid} ID ${NAME} ACTION Unmount
        return 1
    fi

    # Successfully unmounted
    logprint "Sucessfully unmounted ${NASMOUNTPOINT}" 
    change_state "unmounted" ${status_file}
    change_param STATUS "Disabled" ${status_file}
    change_param INFO "" ${status_file}
    /V2O/bin/send_notification -s NAS -t Unmount Index ${nasid} ID ${NAME}
#    change_param UUID "" ${status_file}

}

check_datastore() {

    nasid=$1


    local status_file=${STATUS_DIR}/nas_${nasid}.status

    if [ -f ${status_file} ] ; then
        . ${status_file}
    else
        return
    fi

    if [ x${STATE} != "xmounted" ] ; then
        return
    fi

    if [ x${STATUS} != "xOk" -a x${STATUS} != "xOffline" ] ; then
        return
    fi

    NASMOUNTPOINT=${NASMOUNTROOT}/${nasid}

    # Read our marker file to see if NFS server is alive. If server
    # inaccessible, this will hang for quite some time and return with an
    # error
    if [ -f ${NASLOCKPREFIX}${nasid}.lck ] ; then
        return
    fi
    /bin/touch ${NASLOCKPREFIX}${nasid}.lck

    /usr/bin/wc ${NASMOUNTPOINT}/mount.date > /dev/null 2>&1
    if [ x$? = "x0" ]; then
        if [ x${STATUS} = "xOffline" ] ; then
            change_param STATUS "Ok" ${status_file}
            change_param INFO "" ${status_file}
            /V2O/bin/send_notification -s NAS -t Online Index ${nasid} ID ${NAME}
        fi
        /bin/df ${NASMOUNTPOINT} > ${STATUS_DIR}/nas_${nasid}.df.tmp
        /bin/mv ${STATUS_DIR}/nas_${nasid}.df.tmp ${STATUS_DIR}/nas_${nasid}.df

        /bin/rm -f ${NASLOCKPREFIX}${nasid}.lck

        return
    fi

    # Ok... something is wrong.
    if [ x${STATUS} != "xOffline" ] ; then
        change_param STATUS "Offline" ${status_file}
        change_param INFO "\"Server Unreachable\"" ${status_file}
        /V2O/bin/send_notification -s NAS -t Offline Index ${nasid} ID ${NAME}
    fi
    /bin/rm -f ${NASLOCKPREFIX}${nasid}.lck

}

# Possible STATEs:
# init
# mounted
# unmounted
# mounting
# unmounting
# retry
# retryunmount

mount_nas() {
    local nasid=$1
    local status_file=${STATUS_DIR}/nas_${nasid}.status


    if [ x${STATE} = "xmounted" ] ; then
        ${THIS_SCRIPT_PATH} check ${nasid} > /dev/null 2>&1 &
        return
    fi

    local need_to_mount=0

    if [ x${STATE} = "xinit" ] ; then
        change_param STATUS "Mounting" ${status_file}
        need_to_mount=1
    fi

    if [ x${STATE} = "xretry" ] ; then
        need_to_mount=1
    fi

    if [ x${STATE} = "xunmounted" ] ; then
        change_param STATUS "Mounting" ${status_file}
        need_to_mount=1
    fi

    # Invoke mounting in background
    if [ $need_to_mount = 1 ] ; then
        change_state "mounting" ${status_file}
        /bin/cp ${CONFIG_DIR}/nas_${nasid}.conf ${STATUS_DIR}/nas_${nasid}.conf
        ${THIS_SCRIPT_PATH} mount ${nasid} > /dev/null 2>&1 &
    fi

}


unmount_nas() {
    local nasid=$1
    local status_file=${STATUS_DIR}/nas_${nasid}.status

    if [ x${REMOUNT} = "xtrue" ] ; then
        if [ x${STATE} = "xunmounted" -o \
            x${STATE} = "xretry" ] ; then
            logprint "Clearing REMOUNT flag: No need: STATE=${STATE}"
            /bin/grep -v REMOUNT ${status_file} > ${STATUS_DIR}/tmp_status_file
            /bin/mv -f ${STATUS_DIR}/tmp_status_file ${status_file}
            REMOUNT=false
            if [ x${ENABLE} = "x1" ] ; then
                mount_nas ${nasid}
            fi
            return
        fi
    fi

    # If delete request, and we are done unmounting, cleanup
    if [ x${ACTION} = "xdelete" ] ; then
        if [ x${STATE} = "xunmounted" -o \
            x${STATE} = "xretry" ] ; then
            logprint "Cleaning up deleted config for ${nasid}"
            /V2O/bin/send_notification -s NAS -t Delete Index ${nasid} ID ${NAME}
            /bin/rm ${CONFIG_DIR}/nas_${nasid}.conf
            /bin/rm ${status_file}
            /bin/rm -f ${STATUS_DIR}/nas_${nasid}.df
            return
        fi
    fi


    if [ x${STATE} = "xunmounted" ] ; then
        return
    fi

    local need_to_unmount=0

    if [ x${STATE} = "xinit" ] ; then
        need_to_unmount=1
    fi

    if [ x${STATE} = "xretry" ] ; then
        change_state "unmounted" ${status_file}
    fi

    if [ x${STATE} = "xretryunmount" ] ; then
        need_to_unmount=1
    fi

    if [ x${STATE} = "xmounted" ] ; then
        need_to_unmount=1
    fi

    # Invoke unmounting in background
    if [ $need_to_unmount = 1 ] ; then
        change_state "unmounting" ${status_file}
        /bin/cp ${CONFIG_DIR}/nas_${nasid}.conf ${STATUS_DIR}/nas_${nasid}.conf
        ${THIS_SCRIPT_PATH} unmount ${nasid} > /dev/null 2>&1 &
    else
        change_param STATUS "Disabled" ${status_file}
        change_param INFO "" ${status_file}
    fi

}

# Monitor the progress of a specific NAS mointpoint
monitor_status() {

    local nasid=$1
    local status_file=${STATUS_DIR}/nas_${nasid}.status

    #logprint "Monitoring ${nasid}"

    # Create new status file if it does not exist
    if [ ! -f ${status_file} ] ; then
        echo "ID=${nasid}" > ${status_file}
        echo "STATE=init" >> ${status_file}
        echo "STATUS=unknown" >> ${status_file}
        echo "SERVER=${NFS_SERVER}:${REMOTE_DIR}" >> ${status_file}
        echo "MOUNTPOINT=${NASMOUNTROOT}/${nasid}" >> ${status_file}
        echo "INFO=" >> ${status_file}
        echo "READONLY=0" >> ${status_file}
        echo "NAME=${NAME}" >> ${status_file}
    fi


    # If config file is new (re-written by menu apply handler), 
    # insert handled marker
    REMOUNT=false
    if [ x${HANDLED} != "xtrue" ] ; then
        logprint "Detected new config for ${nasid}"
        if [ x${ACTION} = "xmodify" ] ; then
            logprint "Setting REMOUNT flag."
            /bin/grep -v REMOUNT ${status_file} > ${STATUS_DIR}/tmp_status_file
            echo REMOUNT=true >> ${STATUS_DIR}/tmp_status_file
            /bin/mv -f ${STATUS_DIR}/tmp_status_file ${status_file}
        fi
        
        change_param SERVER "${NFS_SERVER}:${REMOTE_DIR}" ${status_file}
        change_param MOUNTPOINT "${NASMOUNTROOT}/${nasid}" ${status_file}
        change_param NAME "${NAME}" ${status_file}

        echo HANDLED=true >> ${CONFIG_DIR}/nas_${nasid}.conf
    fi

    # Source the status file
    . ${status_file}

    # Start mount/unmount process as necessary
    if [ -f ${STOPPED_FILE} -o \
        x${ENABLE} = x0 -o \
        x${REMOUNT} = "xtrue" -o \
        x${ACTION} = "xdelete" ] ; then
        unmount_nas ${nasid}
    else
        mount_nas ${nasid}
    fi

}

start() {

    if [ ! -f ${STOPPED_FILE} ] ; then
        show "NAS already started!!"
        return 0
    fi
    /bin/rm -f ${STOPPED_FILE}

    show "Starting NAS..."

    watch

    
}

stop() {
    if [ -f ${STOPPED_FILE} ] ; then
        show "NAS  already stopped!!"
        return 0
    fi

    touch ${STOPPED_FILE}
    show "Stopping NAS..."

    watch

}

watch() {
    if [ -f ${WATCHED_FILE} ] ; then
        return 0
    fi
    /bin/touch ${WATCHED_FILE}

    ls ${CONFIG_DIR}/nas_*.conf
    if [ x$? = "x0" ]; then
        if [ ! -f /var/run/rpcbind.lock ]; then
            # Start portmapper/rpcbind for NFS access
            logger "Starting Portmapper"
            /sbin/rpcbind
        fi
        # Replace ID with NASID in conf file
        for config_file in `ls ${CONFIG_DIR}/nas_*.conf` ; do
            
            # Source the config file
            HANDLED=
            . ${config_file}
            
            monitor_status ${ID}
        done
    else
        if [ -f /var/run/rpcbind.lock ]; then
            # Stop portmapper/rpcbind for NFS access
            logger "Terminating Portmapper"
            killall rpcbind
            /bin/rm -f /var/run/rpcbind.*
        fi
    fi 
    
    /bin/rm -f ${WATCHED_FILE}

}


case "$1" in
  start)
        start
        ;;

  stop)
        stop
        ;;

  restart)
        stop
        start
        ;;

  watch)
        watch
        ;;

  mount)
        # Get NAS ID from command line
        NASID=$2
        if [ -z ${NASID} ] ; then
            quitwitherror "Provide ID for which NAS needs to be mounted"
        fi
        mount_datastore $NASID
        ;;

  unmount)
        # Get NAS ID from command line
        NASID=$2
        if [ -z ${NASID} ] ; then
            quitwitherror "Provide ID for which NAS needs to be set up."
        fi
        unmount_datastore $NASID
        ;;

  check)
        # Get NAS ID from command line
        NASID=$2
        if [ -z ${NASID} ] ; then
            quitwitherror "Provide ID for which NAS needs to be set up."
        fi
        check_datastore $NASID
        ;;

  hup)
        watch
        ;;

  *)
        echo "Usage: $0 [start|stop]"
        exit 1

esac
