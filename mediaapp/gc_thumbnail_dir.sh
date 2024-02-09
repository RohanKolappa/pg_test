#!/bin/sh

# Clean up script for thumbnail POST directories

# Source common library
. /etc/scripts/common.rc

  
############## Clean up the thumbnail directories ##############
cleanup_live() {

    local base_live_tn_dir="/data/tncache/live"
    
    #Expiry time for thumbnail dir in seconds.
    local expiry=604800	
    
    for source_dir in `ls ${base_live_tn_dir}/`;
    do
        local source_dir_path="${base_live_tn_dir}/${source_dir}"
        
        # Should clean the Directory?
        check_delete_dir ${source_dir_path}
        local ret=$?
        if [ "x${ret}" != "x0" ]; then
            logprint "Thumbnail GC cleanup_live: Deleting '${source_dir_path}'"
            /bin/rm -rf ${source_dir_path}
        fi
    done
    
    return 0
}

############## Delete Dir ##############
check_delete_dir() {           
    
    local source_dir_path="${1}"
    if [ -z ${source_dir_path} ] ; then
        logprint "No Source Dir to delete!"
        return 1
    fi
    
    local access_log="${source_dir_path}/access.log"
        
    # Does access log exist?
    if [ ! -f ${access_log} ]; then
        logprint "Access log file absent!"
        return 1
    fi

    local curr_time=`date +%s`
    local lastaccess=`cat ${access_log}`
    local is_number=`echo ${lastaccess} | egrep ^[[:digit:]]+$`
            
    # Does access log contain a valid number?
    if [ "${is_number}" = "" ]; then
        logprint "Access log invalid value!"
        return 1
    fi

    # Is last access <= 0?
    if [ ${is_number} -le 0 ]; then
        logprint "Access log invalid number!"
        return 1
    fi
    
    # Is last access > current time?
    if [ ${is_number} -ge  ${curr_time} ]; then
        logprint "Access log invalid number!"
        return 1
    fi
                  
    # Is last acessed time older than expiry?        
    local diff=`expr ${curr_time} - ${is_number}`
    if [ ${diff} -gt ${expiry} ]; then
        logprint "Thumbnail dir expired"
        return 1
    fi
    
    return 0
}

action_name="$1"

case "$1" in

    cleanup_live)
    cleanup_live   
    RETVAL="$?"
    ;;

    *)

    echo "Usage: $0 [ cleanup_live ]"
    RETVAL=1
esac

exit $RETVAL
  