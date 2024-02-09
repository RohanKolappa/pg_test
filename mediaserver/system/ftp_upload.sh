#!/bin/sh

# Source common library
. /etc/scripts/common.rc

RETVAL=0

# This is to support backward compatibility
# If there is no 4th argument then a default folder is chosen
# else the folder that is passed as argument is picked up
if [ "x$4" = "x" ]; then    
    _FOLDER_IPVS_LIVEAREA="/data/sedna_dbsnapshot"
else 
    _FOLDER_IPVS_LIVEAREA=$4
fi

_FOLDER_FTP_WORKAREA="${_FOLDER_IPVS_LIVEAREA}/ftpworkarea"
_FILE_LOCK_FTP_UPLOAD_INSTANCE="${_FOLDER_IPVS_LIVEAREA}/lock_ftpupload.instance" 
_FILE_COMMAND_OUTPUT="${_FOLDER_IPVS_LIVEAREA}/command_output.txt"

logprint() {
	logger "$1"
	echo "$1"
}
logshow() {
	echo "$1"
}

cleanup() {
    /bin/rm -rf ${_FOLDER_FTP_WORKAREA}
}
 
########## check_lock / un_lock ##########
check_lock() {

    lock_file=$1
    if [ -z $lock_file ] ; then
        echo "ERROR: lock file name Argument is empty "
        return 1
    fi

    # Check if we are already running
    if [ -f $lock_file ]; then
        #get time difference in seconds
        filedate=`date -r "${lock_file}"  +%s`
        curdate=`date +%s`
        diff=`expr $curdate - $filedate`

        # Make sure that the process that created 
        # this file is still around
        pid=`cat ${lock_file}`
        if [ "$$" = "${pid}" ] ; then
            #should never come here. Same script is tring to acquire lock again.
            #logshow "'$0' Instance lock has been already aquired."
            logshow " "    
        else
            kill -0 $pid > /dev/null 2>&1
            if [ "x$?" = "x0" ]; then
                logprint "ERROR: Busy: Another instance of '$0' is running (time=${diff} secs, prev_pid=${pid})"
                return 1
            fi
            logshow "WARNING: Previous instance of '$0' attempt exited prematurely?? (time=${diff} secs, prev_pid=${pid})"
        fi
        /bin/rm -f ${lock_file}
    fi
    

    # Now write current process's PID to the lock file.
    # Use noclobber to avoid a potential race condition
    if set -o noclobber; echo $$ > "${lock_file}" ; then
        # Lock successfully acquired
        #set +o noclobber
        return 0
    fi
    #set +o noclobber
    logprint "ERROR: Busy: Another instance of '$0' is still running..."
    return 1
}

release_lock() {
    lock_file=$1
    if [ -z $lock_file ] ; then
        logshow "ERROR: lock file name Argument is empty "
        return 0 
    fi
    
    #what was the pid released.
    #pid=`cat ${lock_file}`    
    #logshow "Releasing lock (pid=${pid})."
    
    # Check if we are already running
    # Remove the lock file
    /bin/rm -f ${lock_file}
    return 0
}

ftpuploadfile(){

    # lock
    check_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE || return 1

    # Are we really on a dms??
    if [ ! -f /etc/config/sw_profile_dms.txt ] && [ ! -f /etc/config/sw_profile_ms.txt ] ; then
        logerror "ERROR: This does not seem to be a Management Server device."
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi
    
        
    # check all three arguments are available
    # Get the database name from command line
    srcFilePath_orig=$1
    ftpUrl=$2
    destFileName=$3

    if [ -z ${srcFilePath_orig} -o -z ${ftpUrl}  -o -z ${destFileName} -o ! -f ${srcFilePath_orig} ] ; then
        showerror "ERROR: Invalid parameters. ftp upload requires three parameters - srcFilePath, ftpUrl, destFileName."
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi


    ################ ftp ############
    #is valid ftp url
    ftpStr=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\1/'`
    if [ x${ftpStr} != "xftp://" ] ; then
        showerror "ERROR: The URL for uploading has to start with \"ftp://\""
        echo "Aborted."
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi
    authenticatedStr=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\(.*\)\:\(.*\)\(@\)\([^\/]*\)\(.*\)/\4/'`       
    if [ x${authenticatedStr} = "x@" ] ; then                                                              
	ftpUser=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\(.*\)\:\(.*\)\(@\)\([^\/]*\)\(.*\)/\2/'`       
	ftpPassword=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\(.*\)\:\(.*\)\(@\)\([^\/]*\)\(.*\)/\3/'`       
	ftpHost=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\(.*\)\:\(.*\)\(@\)\([^\/]*\)\(.*\)/\5/'`       
	ftp_dir=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\(.*\)\:\(.*\)\(@\)\([^\/]*\)\(.*\)/\6/'`        
    else                                                                                                   
	ftpHost=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\2/'`                          
	ftp_dir=`echo ${ftpUrl} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\3/'`                           
    fi                    
    

    # Check the hostname/ip from the URL
    if [ -z ${ftpHost} ] ; then
        showerror "ERROR: Hostname/ip is not specified correctly in the url"
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi

    # Check the upload directory path from command line
    if [ -z ${ftp_dir} ] ; then
        showerror "ERROR: Correct directory to ftp the file is not specified in the url"
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi
    
    # Create directory if it does not exist
    /bin/rm -rf ${_FOLDER_FTP_WORKAREA}
    /bin/mkdir -p ${_FOLDER_FTP_WORKAREA}
    /bin/rm -f $_FILE_COMMAND_OUTPUT

    #copy the file to temp area before upload 
    /bin/cp -f  ${srcFilePath_orig} ${_FOLDER_FTP_WORKAREA}/${destFileName}
    srcFilePath="${_FOLDER_FTP_WORKAREA}/${destFileName}"   

    # Copy database from workarea to final location
    # Upload the file.
    if [ -z ${ftpUser} ] ; then
    	/home/onair/bin/ncftpput -r 0 -t 300 ${ftpHost} ./${ftp_dir} ${srcFilePath} > $_FILE_COMMAND_OUTPUT 2>&1
    else
    	/home/onair/bin/ncftpput -r 0 -t 300 -u "${ftpUser}" -p "${ftpPassword}" ${ftpHost} ./${ftp_dir} ${srcFilePath} > $_FILE_COMMAND_OUTPUT 2>&1
    fi

    if [ x$? != "x0" ]; then
        sed -e "s/ncftp[^:]*: //" $_FILE_COMMAND_OUTPUT
        showerror "Failed to upload \"${srcFilePath}\" to ftp \"${ftpUrl}\""
        cleanup
        release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
        exit 1
    fi

    logprint "Successfully uploaded '${srcFilePath_orig}' to '${ftpUrl}/${destFileName}' "

    cleanup

    # unlock
    release_lock $_FILE_LOCK_FTP_UPLOAD_INSTANCE
    return 0

}
#case "$1" in
#  ftpuploaddb)
#        ftpuploaddb "$2" "$3" "$4"
#		RETVAL="$?"
#		;;
# 
#  *)

#  echo "Usage: $0 [ftpuploaddb]"
#  RETVAL=1
#esac
#exit $RETVAL

ftpuploadfile "$1" "$2" "$3"
RETVAL="$?"
exit $RETVAL
