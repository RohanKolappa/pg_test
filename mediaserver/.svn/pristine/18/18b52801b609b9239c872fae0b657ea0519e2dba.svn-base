#!/bin/sh
 
# Source common library
. /etc/scripts/common.rc
  
NASMOUNTROOT="/data1/DSS/data/portal/site/offload/NAS"
NASMOUNTOPTIONS="soft,nolock"
NFSLOG="/var/run/nfs.offload.log"
_SEND_NOTIFICATION_EXEC="send_notification"
_SEND_NOTIFICATION_SRC="NfsUpload"
_TYPE_NFS_COMPLETE="NfsUploadComplete"
_TYPE_NFS_ERROR="NfsUploadError"

############## send_notification ##############
mediaroom_send_notif() {
    #send notification to middleware  
    local event_type="${1}"
    local clientID="${2}"
    local error="${3}"

    local nohup_log="/tmp/mediaroom_send_notif.log"
    /bin/rm -f ${nohup_log}

    ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "clientID" "${clientID}"  "error" "${error}"    > ${nohup_log} 2>&1 
    if [ "x$?" != "x0" ]; then
        result_error=`cat ${nohup_log} 2>&1`
        logprint "Failed to send_notification : ${event_type} error=${error} clientID=${clientID} "   
    fi
    return 0  
}

mediaroom_send_upload_notif() {
      #send notification to middelware  
      local event_type="${1}"
      local clientID="${2}"
      local dirNID="${3}"
      local fileUUID="${4}"
      local error="${5}"

	
      local nohup_log="/tmp/mediaroom_send_upload_notif.log"
      /bin/rm -f ${nohup_log}

      ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "clientID" "${clientID}"  "error" "${error}"  "dirNID" "${dirNID}" "fileUUID" "${fileUUID}" > ${nohup_log} 2>&1 
      if [ "x$?" != "x0" ]; then
          result_error=`cat ${nohup_log} 2>&1`
          logprint "Failed to send_notification : ${event_type} error=${error} clientID=${clientID} "   
      fi
      return 0  
  }

############## NFS Upload ##############  
nfs_upload() {
	local tar_file="${1}"
    local nas_url="${2}"
    local clientID="${3}"
    local format="${4}"
    local zipPath="${5}"
    local standard_media_filename="${6}" 
    local stream_type="${7}"
    local dirNID="${8}"
    local fileUUID="${9}"
        
    local return_code=0
    
    local error_desc=""
    if [ -z ${tar_file} ] ; then
        error_desc="ERROR: Destination File name to nfs_upload is not provided. Aborting nfs upload..."
    elif [ -z ${nas_url} ] ; then
        error_desc="ERROR: Nas URL is not provided to nfs_upload. Aborting nfs upload..."
    elif [ -z ${clientID} ] ; then
        error_desc="ERROR: clientID argument to nfs_upload is not provided. Aborting nfs_upload..."
    fi
    if [ ! -z $error_desc ] ; then
        logprint "${error_desc}"
        mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
        return 1
    fi
      
    #is valid nfs url
    nfsStr=`echo ${nas_url} | sed -e 's/^\(nfs:\/\/\)\([^\/]*\)\(.*\)/\1/'`
    if [ x${nfsStr} != "xnfs://" ] ; then
        local error_desc="ERROR: The URL for uploading has to start with \"nfs://\""
        logprint "${error_desc}"
        mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
        return 1
    fi
    
    #nfsType=`echo ${nas_url} | sed -e 's/^\(nfs\)\([^\/]*\)\(.*\)/\1/'` 
    nfsHost=`echo ${nas_url} | sed -e 's/^\(nfs:\/\/\)\([^\/]*\)\(.*\)/\2/'`                          
    nfs_dir=`echo ${nas_url} | sed -e 's/^\(nfs:\/\/\)\([^\/]*\)\(.*\)/\3/'` 
	
    /bin/mkdir -p ${NASMOUNTROOT}
    /bin/rm -f ${NFSLOG}
    
    # If something is already mounted at this mount point, unmount it first
    /bin/mount | grep "type nfs " | grep ${NASMOUNTROOT} > /dev/null 2>&1
    if [ x$? = "x0" ]; then
        logerror "Some directory is already mounted at ${NASMOUNTROOT}..Trying to Unmount" 
        /bin/umount ${NASMOUNTROOT} > ${NFSLOG} 2>&1
    fi
    
    mountCmd="/bin/mount -t nfs -o ${NASMOUNTOPTIONS} ${nfsHost}:${nfs_dir} ${NASMOUNTROOT}"
    logprint "Mount: ${mountCmd}"
    ${mountCmd} > ${NFSLOG} 2>&1
    local ec=$?

    # Did we really succeed in mounting??
    /bin/mount | grep "type nfs" | grep ${NASMOUNTROOT} > /dev/null 2>&1
    if [ x$? != "x0" -o x$ec != "x0" ]; then
        cat ${NFSLOG} | logger
        local error_desc="Failed to mount : ${nas_url} at ${NASMOUNTROOT}"
        logprint "${error_desc}"
        mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
        return 1
    fi
    
    logprint "Mounted ${nas_url} at ${NASMOUNTROOT}"
    
    if [ "${format}" != "Standard" ] || [ "${stream_type}" = "V2D" ] || [ "${stream_type}" = "UDP" ]; then
    
        # Copy media to NAS
        logprint "Copy: /bin/cp ${tar_file} ${NASMOUNTROOT}/"
        /bin/cp ${tar_file} ${NASMOUNTROOT}/>  ${NFSLOG} 2>&1
        local ec=$?

	    local error_desc=""
        # Did we succeed in copying??
        if [ x$ec != "x0" ]; then
            cat ${NFSLOG} | logger
            error_desc="Failed to Copy : ${tar_file} at ${nas_url}"
            logprint "${error_desc}"
            #mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
            return_code=1  
        fi
    fi
    
    
    
    if [ "${format}" != "Proprietary" ] && [ "${stream_type}" != "V2D" ] && [ "${stream_type}" != "UDP" ]; then
    	
    	# Copy standard media to NAS
        logprint "Copy: /bin/cp ${zipPath} ${NASMOUNTROOT}/"
        /bin/cp ${zipPath} ${NASMOUNTROOT}/ >  ${NFSLOG} 2>&1
        local ec=$?

	    local error_desc=""
        # Did we succeed in copying??
        if [ x$ec != "x0" ]; then
            cat ${NFSLOG} | logger
            error_desc="Failed to Copy :${tar_file} at ${nas_url}"
            logprint "${error_desc}"
            #mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
            return_code=1
        fi
    
    fi
    
    # Unmount
    logprint "Unmount: /bin/umount ${NASMOUNTROOT}"
    /bin/umount ${NASMOUNTROOT} > ${NFSLOG} 2>&1
    local ec=$?
    
    # Did we really unmount successfully??
    /bin/mount | grep "type nfs " | grep ${NASMOUNTROOT} > /dev/null 2>&1
    if [ x$? = "x0" -o x$ec != "x0" ]; then
        cat ${NFSLOG} | logger
        logerror "Failed to unmount ${NASMOUNTROOT}.. Trying force unmount"
    
        /bin/umount -f ${NASMOUNTROOT} > ${NFSLOG} 2>&1 
    fi
    
    # Successfully completed?
    if [ ! -z $error_desc ] ; then
        mediaroom_send_notif "${_TYPE_NFS_ERROR}"  "${clientID}" "${error_desc}"
    else
        mediaroom_send_upload_notif "${_TYPE_NFS_COMPLETE}"  "${clientID}" "${dirNID}" "${fileUUID}" "${error_desc}"
    fi
    
    return ${return_code}
}
  
  
  
action_name="$1"

case "$1" in

    nfs_upload)
    nfs_upload  "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "$10"
    RETVAL="$?"
    ;;
  
    *)
  
    echo "Usage: $0 [ nfs_upload ]" 
    RETVAL=1
  
esac
  
exit $RETVAL
