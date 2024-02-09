 #!/bin/sh

 # global
 . /etc/scripts/database_management_variables.rc
 
 _FOLDER_IPVSDB_SERVER_DATA_SEDNA_NATIVE="/data/sednaserver/sedna-db"
 _FOLDER_IPVSDB_SERVER_DATA_EXIST_XMLDB="/data/existdbserver"
 DATABASE_NAME="msdb"

 ############## import _db_server.rc for DB_SERVER=sedna_native/sedna_xmldb/exist_xmldb ############## 
 . /etc/scripts/_db_server.rc 
 ############## import _db_server.rc for DB_SERVER=sedna_native/sedna_xmldb/exist_xmldb ############## 
 if [ "$DB_SERVER" = "sedna_native" -o "$DB_SERVER" = "sedna_xmldb" ] ; then
     _FOLDER_IPVSDB_SERVER_DATA="${_FOLDER_IPVSDB_SERVER_DATA_SEDNA_NATIVE}"
     _FOLDER_IPVSDB_SERVER_MSDB="${_FOLDER_IPVSDB_SERVER_DATA}/data/${DATABASE_NAME}_files"
 else
     _FOLDER_IPVSDB_SERVER_DATA="${_FOLDER_IPVSDB_SERVER_DATA_EXIST_XMLDB}"
     _FOLDER_IPVSDB_SERVER_MSDB="${_FOLDER_IPVSDB_SERVER_DATA}/data"
 fi


 ############## start definations of variables ###########
 RETVAL=0
 
 #flag or indicators
 _FILE_FLAG_MS_ON="/var/run/ms.conf"
 _FILE_FLAG_IPVSDB_SERVER_RUNNING="/var/run/ipvsdb.running"
 _FILE_FLAG_RESTORE_FACTORY="/etc/config/restore.factorydefault"
 _FILE_FLAG_MANUAL_MODE="/var/run/ms.manual"
 _FILE_FLAG_BACKUP_USING_JAVA="/var/run/savedb.non.native"
 
 # FTP 
 _FILE_VAR_FTP_URL_EXPORT="/etc/config/export.database.url"
 _SCRIPT_FTP_UPLOAD="/etc/scripts/ftp_upload.sh"
 
 #status  and flags for 
 _FOLDER_IPVS_DB_BACKUP="/data/sedna_dbsnapshot"
 _FOLDER_IPVS_DB_BACKUP_STATUS="${_FOLDER_IPVS_DB_BACKUP}/status"

#Consistency log location
CONSISTENCY_CHECK_LOG_FILE="/data/existdbserver/logs/existconsistency.log"
 

 ############## import scripts for sedna_native/sedna_xmldb/exist_xmldb ############## 
 . /etc/scripts/${DB_SERVER}.rc
 ############## import scripts for sedna_native/sedna_xmldb/exist_xmldb ############## 
 
################################ ctlbase ###############
 WATCHABLE="false"
 TESTPREFIX=""
 SCRIPT_PREFIX="/etc/scripts/watchdog"
 SERVER_NAME="IPVS-XML-DB"
 SERVER_CTL_FILE_PREFIX="ipvsxmldb"
 SERVER_STATUS_FILE_PREFIX="ipvsxmldbserver"
 THIS_SCRIPT_PATH="/etc/scripts/ipvsxmldbctl.sh"
 source "${SCRIPT_PREFIX}/ctlbase.sh"
################################ ctlbase ###############
 _FILE_REPLICATION_STATUS="/var/status/replication"

 #live snapshots  and restore from 
 CURRENT_SNAPSHOT_NUMBER=1
 MAX_SNAPSHOT=4
 BACKUP_APP_VERSION=3
 #save history upto bytes 1 mb
 MAX_HISTORY=1048576
 #export / import
 DB_FOLDER=ipvs-db
 OLD_DB_FOLDER=sedna-db
 # DB action reset/import/restore/empty
 CREATE_DB_SRC=""
 CREATE_DB_SRC_TGZ=""
 CREATE_DB_WAS_CORRUPTED=""
 
 #factory default path
 _FILE_IPVSDB_FACTORY_TAR="FACTORY_DATABASE"
 _FILE_IPVSDB_IMPORT_TAR="${DBREPOS}/${DBNAME}.tgz"
 
  #status  and flags for 
 _FILE_FLAG_RECREATE_DB="${_FOLDER_IPVS_DB_BACKUP_STATUS}/${DATABASE_NAME}_recreate" 
 _FILE_FLAG_CORRUPTED_DB="${_FOLDER_IPVS_DB_BACKUP_STATUS}/${DATABASE_NAME}_corrupted" 
  #status
 _FILE_VAR_NUMBER_IPVS_DB_BACKUP="${_FOLDER_IPVS_DB_BACKUP}/current.snapshot.number"
 _FILE_STATUS_RESTORE_SCHEDULE="${_FOLDER_IPVS_DB_BACKUP_STATUS}/restore.schedule.status"
 _FILE_STATUS_LAST_CORRUPTED="${_FOLDER_IPVS_DB_BACKUP_STATUS}/last.corrupted.status"
 _FILE_STATUS_LASTBACKUP="${_FOLDER_IPVS_DB_BACKUP_STATUS}/lastbackup.status"
 _FILE_STATUS_LASTRESTORE="${_FOLDER_IPVS_DB_BACKUP_STATUS}/lastrestore.status"
 _FILE_INFO_LASTRESTORED_DB="${_FOLDER_IPVS_DB_BACKUP_STATUS}/lastrestored_db.info"
 _FILE_STATUS_HISTORY="${_FOLDER_IPVS_DB_BACKUP_STATUS}/dbactivity.history.status"
 _FILE_STATUS_MIGRATION_DB_PROGRESS="${_FOLDER_IPVS_DB_BACKUP_STATUS}/migration_db_progress.status"
 _FILE_PRE_MIGRATION_DB_TGZ="${_FOLDER_IPVS_DB_BACKUP_STATUS}/pre_migration_db.tgz"
 
_FORCE_RESET_FILE="/data/existdbserver/forcereset"
 
 ###### file/folder definations end #######
if [ ! -f /etc/config/arm_system ] ; then
    TOTALSYSMEM=`cat /proc/meminfo | grep MemTotal |  sed -r  's/^[^0-9]*([0-9]+).*/\1/'`
fi 

IPVSMEM=-Xmx256m
if [ $TOTALSYSMEM -gt 7000000 ] ; then
    IPVSMEM=-Xmx1024m
fi


 ###### todo: use ipvscomponent for import / export db #######
 IPVS_DBTASK_APP_NAME=ipvsdbtask
 IPVS_DBTASK_HOME=/home/onair/ipvscomponent
 IPVS_DBTASK_LOGDIR=/var/log/ipvscomponent
 IPVS_DBTASK_LIB="${IPVS_DBTASK_HOME}/lib"
 IPVS_DBTASK_TEMP_FOLDER="${WORKAREA}/${DB_FOLDER}"
 IPVS_DBTASK_JAR="${IPVS_DBTASK_LIB}/ipvsstarter_${IPVSDBTASK_APP_NAME}.jar"
 IPVS_COMP_ARGS="${IPVSMEM} -DipvsAppName=ipvsdbtask -DipvscomponentHome=${IPVS_DBTASK_HOME} -Dipvscomponent.lib.dir=${IPVS_DBTASK_LIB} -DdbTaskFolder=${IPVS_DBTASK_TEMP_FOLDER} "

 ############## end definations of variables ###########

 _ha_script="/etc/scripts/watchdog/watch_linuxha.sh"

 ######### parse info file #######
 getCurDate(){
     filedate=`date`
     echo $filedate
 }

 getCountOfRunningProcess() {
     process_name=$1
     if [ -z $process_name ] ; then
         return 0 
     fi
     #ret=`pidof $process_name | wc -w` 
     killall -0 $process_name  > /dev/null 2>&1
     if [ "x$?" != "x0" ]; then
         return 0
     else
         return 1
     fi
 }

 ###########  save and read Schedule CreateDB ##############
 check_createdbSchedule() {
     # This function returns 0 is if db recreation is needed
     if [ ! -f $_FILE_FLAG_RECREATE_DB ] ; then
         # create db is not present. File does not exist _FILE_FLAG_RECREATE_DB 
         return 1
     fi
     #read from file         
     exec<$_FILE_FLAG_RECREATE_DB
     read CREATE_DB_SRC 
     read CREATE_DB_SRC_TGZ 
     read CREATE_DB_WAS_CORRUPTED
     if [ "x${CREATE_DB_SRC}" = "x" ]; then
         return 1
     fi
     if [ "x${CREATE_DB_SRC_TGZ}" = "x" ]; then
         return 1
     fi
     if [ "x${CREATE_DB_WAS_CORRUPTED}" = "x" ]; then
         return 1
     fi
     return 0
 }

 create_history(){
     file_name="$1"
     if [ -z $file_name ] ; then
         return
     fi
     if [ ! -f $file_name ] ; then
         return
     fi
     log_data=`cat ${file_name} 2>&1`
     filedate=`getCurDate "create_history"`

     history_size=`cat $_FILE_STATUS_HISTORY | wc -c`
     if [ $MAX_HISTORY -lt $history_size ] ; then
         /bin/mv -f $_FILE_STATUS_HISTORY "${_FILE_STATUS_HISTORY}_bck"
     fi

     echo -e "\n$filedate ###### $2 ############### " >> $_FILE_STATUS_HISTORY
     if [ ! -z "${log_data}" ] ; then
         echo -e "${log_data} \n " >> $_FILE_STATUS_HISTORY
     fi
 }

 save_createdbSchedule() {
     action="$1"
     db_tgz="$2"
     was_corrupted="false"
     if [ "x$3" = "xtrue" ] ; then
         was_corrupted="true"
     fi

     # Save a marker file to indicate that restore is needed.
     rm -f $_FILE_FLAG_RECREATE_DB 
     echo -e "${action}\n${db_tgz}\n${was_corrupted}" > $_FILE_FLAG_RECREATE_DB

     #save status of schedule. only used for status.
     /bin/mkdir -p ${_FOLDER_IPVS_DB_BACKUP_STATUS}
     /bin/rm -rf $_FILE_STATUS_RESTORE_SCHEDULE


     #set +o noclobber
     ver=`cat /home/onair/server.version 2>&1`
     filedate=` getCurDate "save_createdbSchedule" `

     echo  -e "WAS_CORRUPTED=${was_corrupted}\nSCHEDULED_DATE=\"${filedate}\"\nREASON=${action}\nFILE=${db_tgz}" > $_FILE_STATUS_RESTORE_SCHEDULE
     logprint "Scheduled to restore database (REASON=${action}, FILE=${db_tgz}, WAS_CORRUPTED=${was_corrupted})."

     create_history $_FILE_STATUS_RESTORE_SCHEDULE "SCHEDULED RESTORE ON NEXT STARTUP"
     return 0
 }


 scheduleCreatedb() {
     reason="$1"
     db_tgz="$2"
     was_corrupted="false"
     if [ "x$3" = "xtrue" ] ; then
         was_corrupted="true"
     fi

     if [ -z $reason -o -z $db_tgz ] ; then
         logprint "ERROR: reason and db_tgz file argument required."
         return 1
     fi

     # If the requested db tarball file does not exist, this better be
     # a request for factory defaults
     if [ ! -f $db_tgz -a "${_FILE_IPVSDB_FACTORY_TAR}" != "${db_tgz}" ] ; then
         logprint "ERROR: '${db_tgz}' file does not exist. Can not schedule restore database (REASON=${reason})"
         return 1
     fi

     # Check the file is valid tar
     temp_folder="${WORKAREA}/schedulecdb_${reason}"
     local extract_full="false"
     isValidTar "${db_tgz}" ${temp_folder} $extract_full
     isValid="x$?"
     /bin/rm -rf ${temp_folder}
     if [ "${isValid}" != "x0" ]; then
         # Requested db is invalid. Potentially switch to factory db.
         if [ "$reason" = "restore_lastbackup" -o "$reason" = "restore_snapshot" ]; then
             db_tgz=$_FILE_IPVSDB_FACTORY_TAR 
             # Recheck the initdb script is ok          
             isValidTar "${db_tgz}"  ${temp_folder} $extract_full
             isValid="x$?"
             /bin/rm -rf ${temp_folder}
             # This is not expected to fail since we have switched to factory db
             if [ "${isValid}" != "x0" ]; then
                 logprint "ERROR: Failed to schedule restore '${reason}' . The '${db_tgz}' tar is not valid."
                 return 1
             fi
         else
             logprint "ERROR: Failed to schedule restore '${reason}' . The '${db_tgz}' tar is not valid."
             return 1
         fi
     fi
     
     # If not already scheduled ... schedule and return... 
     check_createdbSchedule
     if [ "x$?" != "x0" ]; then
         # No prev schedule..
         save_createdbSchedule $reason $db_tgz $was_corrupted
         return 0
     fi

     # Prev schedule exist.. then veify the priority
     # If saved and current reason are same override the saved schedule..     
     if [ "${CREATE_DB_SRC}" = "${reason}" ]; then
         save_createdbSchedule $reason $db_tgz $was_corrupted
         return 0
     fi

     logprint "New schedule (REASON=${reason} FILE=${db_tgz})... Already scheduled (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ})."      

     if [ "$reason" = "restore_premigrationdb" ]; then
         # Higher priority for restore_premigrationdb than  restore_factory / restore_factory
         # Question: why this double compare of the same thing?
         if [ "$CREATE_DB_SRC" = "restore_factory" -o "$CREATE_DB_SRC" = "restore_factory" ]; then
             save_createdbSchedule $reason $db_tgz $was_corrupted
             return 0
         fi
     fi

     # tgz exits ? old schedule restore_lastbackup
     if [ "$CREATE_DB_SRC" = "restore_lastbackup" -a "$reason" = "restore_factory" ]; then
         if [ ! -f $CREATE_DB_SRC_TGZ   ]; then
             logprint "WARNING: Previous Restore database failed (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ}). File '${CREATE_DB_SRC_TGZ}' does not exists. Retry restore $reason...."      
             save_createdbSchedule $reason $db_tgz $was_corrupted
             return 0
         fi  
         logprint "WARNING: Previous Restore database failed (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ}). Retry restore ...."      
         return 0
     fi
     # Should have lowest priority, can not overwrite the prev schedule
     if [ "$reason" = "restore_lastbackup" -o "$reason" = "restore_snapshot" -o "$reason" = "restore_factory" -o "$reason" = "empty" -o "$reason" = "restore_premigrationdb" ]; then
         logprint "ERROR: Can not schedule '${reason}'. Restore database was already scheduled (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ})."      
         return 1
     fi

     # Prev schedule exists
     save_createdbSchedule $reason $db_tgz $was_corrupted
     return 0
 }

 #### empty reset/factory import lastimport create_snapshot lastbackup
 schedule_createdbIfEmpty() {
     # If migration in progress than restore db from pre-migration database 
     if [ -f $_FILE_STATUS_MIGRATION_DB_PROGRESS ] ; then
         if [ ! -f $_FILE_PRE_MIGRATION_DB_TGZ ] ; then
             logprint "WARNING:database migration was incomplete, can not find pre_migration_database '${_FILE_PRE_MIGRATION_DB_TGZ}' file."
             /bin/rm -f $_FILE_STATUS_MIGRATION_DB_PROGRESS
         else
             logprint "WARNING:database migration was incomplete, schedule pre_migration_database restore from '${_FILE_PRE_MIGRATION_DB_TGZ}' file."
             #/bin/rm -f $_FILE_STATUS_MIGRATION_DB_PROGRESS
             scheduleCreatedb "restore_premigrationdb" "${_FILE_PRE_MIGRATION_DB_TGZ}"
             return $?
         fi
     fi     

     # call native for sedna_native/sedna_xmldb/exist_xmldb
     native_schedule_createdbIfEmpty
     local native_ret=$?
     if [ "x$native_ret" != "x0" ]; then
         scheduleCreatedb "empty" "${_FILE_IPVSDB_FACTORY_TAR}"
         return $?
     fi

     return 0    
 }

# This function is a request for the db to go to factory defaults
# If an argument is provided, that serves as a "reason" string to
# go to factory defaults
 reset_implementation(){
     reason="reset"
     if [ "x$1" != "x" ] ; then
         reason="$1"
     fi
     scheduleCreatedb "$reason" "${_FILE_IPVSDB_FACTORY_TAR}" "$2"
     if [ "x$?" != "x0" ]; then
         logprint "ERROR: Failed trying factory reset. "
         return 1
     fi
     return 0
 }

 schedule_restoreSnapshot() {
     local reason="lastbackup"
     if [ "x$1" != "x" ] ; then
         reason="$1"
     fi

     local was_corrupted="false"
     if [ "x$2" = "xtrue" ] ; then
         was_corrupted="true"
     fi

     # snapshot must exist  
     if [ -f "${_FILE_VAR_NUMBER_IPVS_DB_BACKUP}" ] ; then
         #previous back up point     
         CURRENT_SNAPSHOT_NUMBER=`cat $_FILE_VAR_NUMBER_IPVS_DB_BACKUP 2>&1`
         FOLD=${_FOLDER_IPVS_DB_BACKUP}/data_${CURRENT_SNAPSHOT_NUMBER}
         db_tgz="${FOLD}/msdb.tgz"
         scheduleCreatedb "${reason}" "${db_tgz}" "${was_corrupted}"
         local ret="$?"
         #schedule take over if ha enabled and active
         if [ "x${ret}" = "x0" ]; then
             $_ha_script scheduleTakeOverIfActive
         fi
         return "$ret"
     fi    

     reset "$reason" "${was_corrupted}"
     return "$?"
 }

 schedule_importdb() {
     local import_name="$1"
     if [ -z "$import_name" ] ; then
         logprint "ERROR: Import file name argument is empty."
         return 1
     fi

     local db_tgz="$import_name"
     if [ ! -f "${db_tgz}" ] ; then
         db_tgz="${DBREPOS}/${import_name}" 
         if [ ! -f "${db_tgz}" ] ; then
             db_tgz="${DBREPOS}/${import_name}.tgz" 
         fi
     fi

     if [ ! -f  "${db_tgz}" ] ; then
         logprint "ERROR: Import '${db_tgz}' file does not exist."
         return 1
     fi

     #todo: before tring to delete db check if the new database is valid
     local temp_folder="${WORKAREA}/schedule_importdb"
     local extract_full="false"
     isValidTar "${db_tgz}" ${temp_folder} $extract_full
     local isValid="x$?"
     /bin/rm -rf ${temp_folder}
     if [ "${isValid}" != "x0" ]; then
         logprint "ERROR: Failed to schedule restore '${db_tgz}'. The tar is not valid."
         return 1
     fi

     scheduleCreatedb "import" $db_tgz
     local ret="$?"
     #schedule take over if ha enabled and active
     if [ "x${ret}" = "x0" ]; then
         $_ha_script scheduleTakeOverIfActive
     fi
     return "$ret"   
 }

 importdb(){
     check_lock || return 1

     local save_to_tgz=$1
     if [ -z "${save_to_tgz}" ] ; then
         logprint "ERROR: Import db name is empty. Aborting import......"
         # unlock
         release_lock
         return 1
     fi

     schedule_importdb "${save_to_tgz}"
     if [ x$? != "x0" ]; then
         logprint "ERROR: Failed trying import."
         # unlock
         release_lock
         return 1
     fi

     stop_implementation
     sleep 3

     start_implementation
     sleep 3

     release_lock
     return 0

 }

 ################## snapshot/ exportdb ###################
 exportdb(){

     #file name is required
     local save_to_tgz=$1
     if [ -z ${save_to_tgz} ] ; then
         logprint "ERROR: Export db name is empty. Aborting export..."
         return 1
     fi

     local save_reason="export"
     if [ ! -z $2 ] ; then
         save_reason="$2"
     fi

     check_lock || return 1

     create_snapshot_internal "${save_reason}" "${save_to_tgz}" "$3"
     local ret=$?

     release_lock
     return $ret
 }

 
 create_snapshot_internal() {
     # Server must be running
     if [ ! -f $RUNNING_FILE ] ; then
         logprint "ERROR: ${DB_SERVER_DESC} Server not running. Aborting create snapshot..."
         return 1
     fi

     # If migration is in progress, do not export
     if [ -f $_FILE_STATUS_MIGRATION_DB_PROGRESS ] ; then
         logprint "ERROR: Database migration in progress. Aborting create snapshot..."
         return 1
     fi

     save_reason="manual"
     if [ ! -z $1 ] ; then
         save_reason="$1"
     fi
     if [ "$save_reason" = "Scheduled" ]; then
         if [ ! -f $_FILE_FLAG_MS_ON ]; then
             logprint "ERROR: Aborting create snapshot, MS (XMPP) is not enabled."
             return 1
         fi
     fi

     logprint "Creating snapshot. Please wait... this may take a while."
     logprint "Verifing ${DB_SERVER_DESC} ${DATABASE_DESC} is in ready state to create snapshot. "
     #check if db is in readable state
     #status
     #if [ "x$?" != "x0" ]; then
     #    logprint "ERROR: Failed trying create snapshot. ${DB_SERVER_DESC} ${DATABASE_DESC} is not in ready status for create snapshot."
     #    return 1
     #fi

     # Save snapshot
     save_snapshot "${save_reason}"
     ret=$?
     if [ "x$ret" = "x0" ]; then
         # If 2nd and 3rd argument provided, we also export snapshot to the
         # the given location
         save_to_tgz=$2
         if [ ! -z ${save_to_tgz} ] ; then
             export_snapshot "${save_to_tgz}" "$3"
             ret=$?
         fi
     fi

     return $ret
 }

 create_snapshot() {
     # Lock export / save database 
     check_lock || return 1
     create_snapshot_internal "$1" "$2" "$3" "$4"
     local ret=$?
     release_lock
     return $ret
 }


 save_snapshot(){

     if [ -f "$_FILE_VAR_NUMBER_IPVS_DB_BACKUP" ] ; then
         CURRENT_SNAPSHOT_NUMBER=`cat $_FILE_VAR_NUMBER_IPVS_DB_BACKUP 2>&1`
         CURRENT_SNAPSHOT_NUMBER=`expr $CURRENT_SNAPSHOT_NUMBER + 1`
         #if reached max (4) than reset to 1
         if [ $MAX_SNAPSHOT -lt $CURRENT_SNAPSHOT_NUMBER ] ; then
             CURRENT_SNAPSHOT_NUMBER=1
         fi
     else
         CURRENT_SNAPSHOT_NUMBER=1
     fi

     rm -rf ${_FOLDER_IPVS_DB_BACKUP}/data_${CURRENT_SNAPSHOT_NUMBER}
     mkdir -p ${_FOLDER_IPVS_DB_BACKUP}/data_${CURRENT_SNAPSHOT_NUMBER}
     save_to_tgz=${_FOLDER_IPVS_DB_BACKUP}/data_${CURRENT_SNAPSHOT_NUMBER}/msdb.tgz

     save_reason="unknown"
     if [ ! -z $1 ] ; then
         save_reason="$1"
     fi

     save_db $save_to_tgz "${save_reason}"
     if [ "x$?" != "x0" ]; then
         return 1
     fi  

     /bin/rm -f $_FILE_VAR_NUMBER_IPVS_DB_BACKUP
     echo $CURRENT_SNAPSHOT_NUMBER > $_FILE_VAR_NUMBER_IPVS_DB_BACKUP
     return 0
 }

 export_snapshot() {
     copy_to_tgz=$1
     copy_to_path=1
     if [ ! -z $2 ] ; then
         copy_to_path=0
     fi
     if [ -z $copy_to_tgz ] ; then
         copy_to_tgz="${DBREPOS}/msdb.tgz" 
     else
         if [ $copy_to_path -eq 1 ] ; then
             copy_to_tgz="${DBREPOS}/${copy_to_tgz}.tgz" 
         fi
     fi

     save_to_tgz=""
     if [ -f "$_FILE_VAR_NUMBER_IPVS_DB_BACKUP" ] ; then
         CURRENT_SNAPSHOT_NUMBER=`cat $_FILE_VAR_NUMBER_IPVS_DB_BACKUP 2>&1`
         if [  -z $CURRENT_SNAPSHOT_NUMBER ] ; then
             #save_to_tgz="$_FILE_IPVSDB_FACTORY_TAR"
             logprint "ERROR: Database snapshot does not exist or is invalid. To export snapshot, Please first create one."
             return 1
         fi
         save_to_tgz=${_FOLDER_IPVS_DB_BACKUP}/data_${CURRENT_SNAPSHOT_NUMBER}/msdb.tgz
         if [ ! -f $save_to_tgz ] ; then
             #save_to_tgz="$_FILE_IPVSDB_FACTORY_TAR"
             logprint "ERROR: Database snapshot does not exist '$save_to_tgz' or is invalid. To export snapshot, Please first create one."
             return 1
         fi
         logprint "Exporting snapshot from '$save_to_tgz' to '$copy_to_tgz' "
     else
         save_to_tgz="$_FILE_IPVSDB_FACTORY_TAR"
         logprint "Database Snapshots not found. Exporting factory from '$save_to_tgz' to '$copy_to_tgz' "
     fi

     /bin/cp -f "$save_to_tgz" "$copy_to_tgz"

     return 0

 }
 ################## snapshot/ exportdb ###################
 ###########  save and read Schedule CreateDB ##############
 ftp_upload() {

     local ipvsdb_log="${_FOLDER_IPVS_DB_BACKUP_STATUS}/ftp_file_log" 
     /bin/rm -f $ipvsdb_log

     #if ftp disaled return
     if [ ! -f $_FILE_VAR_FTP_URL_EXPORT ] ; then
         return 0
     fi

     src_file="$1"
     if [ -z "${src_file}" ] ; then
         logprint "ERROR: File path to database sanpshot is not provided. Aborting upload database to ftp ..."
         return 1
     fi

     ftp_url=`cat "$_FILE_VAR_FTP_URL_EXPORT" 2>&1`
     time_stamp=`date +%Y-%b-%d-%H.%M.%S`
     dest_file_name="msdb-${time_stamp}.tgz"

     #${_SCRIPT_FTP_UPLOAD}  ${src_file} ${ftp_url} "${dest_file_name}" >> $ipvsdb_log 2>&1  
     #result=`cat $ipvsdb_log 2>&1`
     #logprint "${result}"

     ${_SCRIPT_FTP_UPLOAD}  ${src_file} "${ftp_url}" "${dest_file_name}"   > $ipvsdb_log 2>&1 &

     return 0

 }

 save_db() {
     save_to_tgz=$1
     if [ -z ${save_to_tgz} ] ; then
         #todo: may be just return. if argument not given. should never happen
         #save_to_tgz="/home/onair/templates/initdb.tgz"
         logprint "ERROR: File name to save db is not provided. Aborting backup database..."
         return 1
     fi

     save_reason="unknown"
     if [ ! -z "$2" ] ; then
         save_reason="$2"
     fi

     backup_print="(DATABASE='${DATABASE_NAME}', BACKUP_REASON=$save_reason, FILE='${save_to_tgz}')"
     logprint "Backup database in progess ${backup_print}. Please wait... this may take a while."

     temp_folder="${WORKAREA}/save_db_${save_reason}"
     /bin/rm -rf ${temp_folder}
     /bin/mkdir -p ${temp_folder}/${DB_FOLDER}
     /bin/mkdir -p ${temp_folder}/db
     /bin/rm -f $save_to_tgz 

     #ipvsdb_log="/tmp/ipvsdb_exp.log"
     ipvsdb_log="${_FOLDER_IPVS_DB_BACKUP_STATUS}/ipvsdb_exp.log"
     /bin/rm -f $ipvsdb_log

     backupdate=` getCurDate "save_db1" `
     if [  -f  $_FILE_FLAG_BACKUP_USING_JAVA ] ; then
         # call java 
         ipvs_comp_args=" ${IPVS_COMP_ARGS} -DdbTaskName=backup "
         CMD_EXEC="/usr/bin/java -server ${ipvs_comp_args} -classpath ${IPVS_DBTASK_JAR} -jar ${IPVS_DBTASK_JAR} "
         eval $CMD_EXEC 

         # add info to the tar
         createBackupInfo ${temp_folder}/db/info.txt
         #cat ${temp_folder}/db/info.txt	
     else
         
         #call native for sedna_native/sedna_xmldb/exist_xmldb	
         native_export_db ${temp_folder}/${DB_FOLDER}
         local native_ret=$?
         if [ "x$native_ret" != "x0" ]; then
             return $native_ret
         fi
	 
         # Now tar it up
         /bin/tar -czvf "${temp_folder}/db/msdb.tgz" -C ${temp_folder} ${DB_FOLDER}   > /dev/null 2>&1     
         if [ "x$?" != "x0" ]; then
             logprint "ERROR: Could not tarzip backup files ${backup_print}."
             return 1
         fi

         /bin/rm -f $ipvsdb_log

         #remove temp folder
         /bin/rm -rf ${temp_folder}/${DB_FOLDER}

         ## add info 
         # Create metadata files
         # add info to the tar
         createBackupInfo ${temp_folder}/db/info.txt
         cat ${temp_folder}/db/info.txt	
     fi

     # tar 'em up
     /bin/tar -C ${temp_folder} -czf $save_to_tgz db  > /dev/null 2>&1
     /bin/rm -rf ${temp_folder}

     #validate
     temp_folder2="${WORKAREA}/save_db2"
     #todo: see if it valid and unzippable
     local extract_full="false"
     isValidTar "$save_to_tgz" ${temp_folder2} $extract_full
     isValid="x$?"
     /bin/rm -rf ${temp_folder2} 
     if [ "${isValid}" != "x0" ]; then
         logprint "ERROR: Failed to backup database ${backup_print}."
         #remove temp folder
         return 1
     fi

     #get version and size
     result=`cat $save_to_tgz | wc -c`
     ver=`cat /home/onair/server.version 2>&1`

     /bin/rm -rf $_FILE_STATUS_LASTBACKUP
     echo -e "VERSION=${ver}\nBACKUPTIME=\"${backupdate}\"\nBACKUP_APP_VERSION=${BACKUP_APP_VERSION}\nBACKUPREASON=${save_reason}\nFILE=${save_to_tgz}\nSIZE=${result}" > $_FILE_STATUS_LASTBACKUP

     logprint "Sucessfully completed database backup (DATABASE='${DATABASE_NAME}', BACKUP_REASON=$save_reason, FILE='${save_to_tgz}', SIZE=$result bytes )"
     create_history $_FILE_STATUS_LASTBACKUP "CREATED LASTBACKUP"

     ftp_upload "${save_to_tgz}"
     return 0
 }

 verify_database_tar(){
     db_tgz="$1"
     if [ -z ${db_tgz} ] ; then
         logprint "ERROR: Invalid file name '${db_tgz}'."
         return 1
     fi
     temp_folder="${WORKAREA}/verify_database_tar"
     local extract_full="false"
     isValidTar  ${db_tgz} ${temp_folder} $extract_full
     local ret="$?" 
     /bin/rm -rf ${temp_folder}
     if [ "x$ret" != "x0" ]; then
         echo "INVALID"
         return 1
     fi

     echo "VALID"
     return "$ret"
 }

 isValidTar() {

     db_tgz="$1"

     if [ -z ${db_tgz} ] ; then
         logprint "ERROR: Invalid file name '${db_tgz}'."
         return 1
     fi

     # Factory defaults request is always assumed ok
     if [ $_FILE_IPVSDB_FACTORY_TAR = $db_tgz ] ; then
         return 0
     fi

     # The file better exist
     if [ ! -f ${db_tgz} ] ; then
         logprint "ERROR: File '${db_tgz}' does not  exists."
         return 1
     fi

     temp_folder="$2"
     if [ -z "$2" ] ; then
         temp_folder="${WORKAREA}"
     fi

     #should ideally move into db specific rc file
     extract_full="$3"
     /bin/rm -rf ${temp_folder}
     /bin/mkdir -p ${temp_folder}/${DB_FOLDER}
     
     /bin/tar -xzf  ${db_tgz} -C ${temp_folder}     > /dev/null 2>&1     
     if [ "x$?" != "x0" ]; then
         logprint "ERROR: Failed to untar   '${db_tgz}'."
         return 1
     fi
     
     if [ ! -f  "${temp_folder}/db/msdb.tgz" ] ; then
         logprint "ERROR: Failed to untar '${db_tgz}'. msdb.tgz not found "
         return 1
     fi

     if [ ! -f  "${temp_folder}/db/info.txt" ] ; then
         logprint "ERROR: Failed to untar '${db_tgz}'. info.txt not found "
         return 1
     fi

     # For the database to be valid, one of these files should exist:
     # ${DB_FOLDER}/db/__contents__.xml
     # ${DB_FOLDER}/create_indexes.xquery
     # ${OLD_DB_FOLDER}/create_indexes.xquery

     # We either physically extract the tar bar, or do a listing to verify that
     # at least one of the above file exists
     if [  "$extract_full" = "true" ] ; then
         #todo: init db tgz and import/export tgz format same 
         logprint "Extracting database from  '${db_tgz}', Please wait... this may take a while  "
         /bin/tar -C ${temp_folder} -xzf "${temp_folder}/db/msdb.tgz"    > /dev/null 2>&1 
         if [ "x$?" != "x0" ]; then
             logprint "ERROR: Invalid database file '${db_tgz}'. failed to untar msdb.tgz."
             return 1
         fi
         logprint "Done Extracting database from  '${db_tgz}'"
         if [ -f  ${temp_folder}/${DB_FOLDER}/create_indexes.xquery -o  -f  ${temp_folder}/${OLD_DB_FOLDER}/create_indexes.xquery  -o -f  ${temp_folder}/${DB_FOLDER}/db/__contents__.xml ] ; then
             return 0
         fi
         logprint "ERROR: Invalid database file '${db_tgz}'. Does not contain DB files."
         return 1
     fi

     /bin/tar -tzvf  ${temp_folder}/db/msdb.tgz ${DB_FOLDER}/db/__contents__.xml    > /dev/null 2>&1     
     if [ "x$?" = "x0" ]; then
         return 0
     fi

     /bin/tar -tzvf ${temp_folder}/db/msdb.tgz ${DB_FOLDER}/create_indexes.xquery   > /dev/null 2>&1     
     if [ "x$?" = "x0" ]; then
         return 0
     fi

     /bin/tar -tzvf ${temp_folder}/db/msdb.tgz  ${OLD_DB_FOLDER}/create_indexes.xquery    > /dev/null 2>&1     
     if [ "x$?" = "x0" ]; then
         return 0
     fi

     logprint "ERROR: Invalid database file '${db_tgz}'. Does not contain DB files."
     return 1
 }

 ################## start-stop and functions using start - stop  re_start , import ,  restore ###################


 cleanup_database() {
     # We should only wipeout existing database if database recreation
     # is needed for any reason

     # First trigger database recreation due to 1) there being 
     # no database at all or 2) a pre-migration needs to be done
     schedule_createdbIfEmpty

     # Check if database recreation is needed
     check_createdbSchedule
     if [ "x$?" != "x0" ]; then
         return 1
     fi

     # There is create db scheduled, so wipe out existing one
     logprint "Restoring database (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ})."
     logprint "Deleting database files."

     # Call native for sedna_native/sedna_xmldb/exist_xmldb	
     native_cleanup_database

     return 0
 }

 check_database() {

     # create DB repository if it does not exist
     /bin/mkdir -p ${DBREPOS}

     check_createdbSchedule
     if [ "x$?" != "x0" ]; then
         return 1
     fi
     
     # Call native for sedna_native/sedna_xmldb/exist_xmldb	
     native_check_database
     return 0
     
 }

 createBackupInfo(){
     local info_file="$1"
     if [ -f $_FILE_INFO_LASTRESTORED_DB ]; then
         /bin/cp -f $_FILE_INFO_LASTRESTORED_DB $info_file
         return 0
     fi
     #exist_xmldb/sedna_xmldb/sedna_native
     version_val=`/bin/cat /home/onair/server.version 2>&1`
     created_val=` getCurDate "save_db2" `
     serviceVersion_val=`xsltproc /etc/scripts/xsl/get_version.xsl /home/onair/objectXml/service.version`
     DBServer_val="${DB_SERVER}"
     backupAppVersion_val="${BACKUP_APP_VERSION}"
     if [ ! -z $2 ]; then
         backupAppVersion_val="$2"
     fi

     rm -rf $info_file
     touch $info_file
     echo "version: ${version_val}" 					>> $info_file
     echo "created: ${created_val}" 					>> $info_file 
     echo "serviceVersion: ${serviceVersion_val} " 		>> $info_file
     echo "DBServer: ${DBServer_val}" 					>> $info_file
     echo "backupAppVersion: ${backupAppVersion_val}" 	>> $info_file
     if [ ! -z $2 ]; then
         echo "upgrade_progress: yes" 	>> $info_file
     fi
     return 0
 }

 getBackupInfo_fromFile(){
     local version_val=`findPropertyInFile_withDefault "$1" "version" "unknown"`
     local created_val=`findPropertyInFile_withDefault "$1" "created" "unknown"`
     local serviceVersion_val=`findPropertyInFile_withDefault "$1" "serviceVersion" "unknown"`
     local DBServer_val=`findPropertyInFile_withDefault "$1" "DBServer" "sedna_native"`
     local backupAppVersion_val=`findPropertyInFile_withDefault "$1" "backupAppVersion" "1"`
     echo "BACKUPTIME=\"${created_val}\"\nBACKUP_VERSION=${version_val}\nBACKUP_SERVICEVERSION=${serviceVersion_val}\nBACKUP_DBSERVER=${DBServer_val}\nBACKUP_APP_VERSION=${backupAppVersion_val}"
 }
 getBackupInfo_factory(){
     local version_val=`/bin/cat /home/onair/server.version 2>&1`
     local created_val=` date -r "/home/onair/templates/factorydb/service.version" ` 
     local serviceVersion_val=`xsltproc "/etc/scripts/xsl/get_version.xsl" "/home/onair/objectXml/service.version"`
     local DBServer_val="${DBServer}"
     local backupAppVersion_val="${BACKUP_DBSERVER}"
     echo "BACKUPTIME=\"${created_val}\"\nBACKUP_VERSION=${version_val}\nBACKUP_SERVICEVERSION=${serviceVersion_val}\nBACKUP_DBSERVER=${DBServer_val}\nBACKUP_APP_VERSION=${backupAppVersion_val}"
 }
 findPropertyInFile_withDefault() {
     info_value=`findPropertyInFile "$1" "$2"`
     len=`echo -n $info_value | wc -c`
     if [ ! -z "$info_value" ]; then
         echo $info_value
         return 0
     fi
     echo $3
     return 1
 }


 findPropertyInFile() {
     info_file=$1
     info_key=$2
     if [ ! -f $info_file ]; then
         echo $prop_value
         return 0
     fi
     cat $info_file | while read read_line
       do
       len=`echo -n $read_line | wc -c`
       if [ $len -gt 1 ]; then
           prop_key=`echo $read_line | cut -f1  -d':'`
           prop_value=`echo $read_line | cut -f2-5  -d':'` 
           prop_value=`echo $prop_value  | sed 's/^ *\(.*\) *$/\1/'`
           if [ $info_key = $prop_key ]; then
               echo $prop_value
               return 0
           fi
       fi
     done
     return 1
 }

 getPropName_info() {
     str=$1
     str1=`echo $str | cut -d ':' -f1`
     echo "$str1"
 }

 getPropValue_info() {
     str=$1

     str_len=`expr length "$str"`
     if [ "x$str_len" = "x0" ]; then
         echo ""
         return
     fi

     key_index=`expr index "$str" ":"`
     if [ "x$key_index" = "x0" ]; then
         echo ""
         return
     fi

     substr_start=`expr $key_index + 1 `
     substr_end=`expr $str_len - $substr_start + 1`

     val=`expr substr  "$str"  $substr_start  $substr_end`
     val=`echo "$val" | sed 's/^ *\(.*\) *$/\1/'`

     echo "$val"
 }
 
 populate_database() {
     check_createdbSchedule
     if [ "x$?" != "x0" ]; then
         return 1
     fi

     # Create info file for last restore 
     /bin/rm -rf $_FILE_INFO_LASTRESTORED_DB
     if [ ${_FILE_IPVSDB_FACTORY_TAR} != ${CREATE_DB_SRC_TGZ} ] ; then

         # Populate database 
         logprint "Importing ${DATABASE_DESC} from '${CREATE_DB_SRC_TGZ}' file. Please wait... this may take a while."

         if [ ! -f $CREATE_DB_SRC_TGZ ] ; then
             # This should not happen.  
             logprint "ERROR: Could not locate file '${CREATE_DB_SRC_TGZ}' file to import from."
             return 1
         fi
         
         temp_folder="${WORKAREA}/populate_database"
         local extract_full="true"
         isValidTar  ${CREATE_DB_SRC_TGZ} ${temp_folder} $extract_full
         if [ "x$?" != "x0" ]; then
             logprint "ERROR: Failed to import from '${CREATE_DB_SRC_TGZ}' file. The tar is not valid."
             /bin/rm -rf ${temp_folder}
             return 1
         fi

         # Now import the db
         if [ -d ${temp_folder}/${OLD_DB_FOLDER} ]; then
             DB_FOLDER="${OLD_DB_FOLDER}"
         fi
         

         # Call native for sedna_native/sedna_xmldb/exist_xmldb	
         native_restore_db "${temp_folder}/${DB_FOLDER}"
         local native_ret=$?
         if [ "x$native_ret" != "x0" ]; then
             return $native_ret
         fi
         
         # Save info 
         createBackupInfo $_FILE_INFO_LASTRESTORED_DB

         logprint "Sucessfully imported ${DATABASE_DESC} from '${CREATE_DB_SRC_TGZ}' file."
         #cat ${temp_folder}/db/info.txt
         backupinfo=` getBackupInfo_fromFile "${temp_folder}/db/info.txt" `
     else
         backupinfo=` getBackupInfo_factory`
         
         #save info 
         createBackupInfo $_FILE_INFO_LASTRESTORED_DB
     fi
     
     #cat $_FILE_INFO_LASTRESTORED_DB

     # Remove the schedule file  
     /bin/rm -rf $_FILE_FLAG_RECREATE_DB

     # Remove the indicate corruption file  
     /bin/rm -rf $_FILE_FLAG_CORRUPTED_DB

     # Get details of schedule
     schedule=`cat $_FILE_STATUS_RESTORE_SCHEDULE 2>&1`
     #/bin/rm -rf $_FILE_STATUS_RESTORE_SCHEDULE

     #get version , get restore date , get backup 
     restoredate=` getCurDate "populate_database" `
     ver=`cat /home/onair/server.version 2>&1`
     serviceVersion=`xsltproc "/etc/scripts/xsl/get_version.xsl" "/home/onair/objectXml/service.version"`

     # Create status file for backup
     /bin/rm -rf $_FILE_STATUS_LASTRESTORE
     echo -e "VERSION=${ver}\nRESTORETIME=\"${restoredate}\"\nSERVICEVERSION=${serviceVersion}\n${schedule}\n${backupinfo}" > $_FILE_STATUS_LASTRESTORE
     
     create_history $_FILE_STATUS_LASTRESTORE "RESTORED DATABASE"

     /bin/rm -rf ${temp_folder}
     return 0
 }

 ## needs lock too 
 check_integrity() {
     # see if the db is ok... 
     native_isDBAccessible
     if [ "x$?" = "x1" ]; then

         #indicate that the db was corrupted
         /bin/rm -rf $_FILE_FLAG_CORRUPTED_DB
         touch $_FILE_FLAG_CORRUPTED_DB

         #create a file for status - to read last corruption happened 
         /bin/rm -rf $_FILE_STATUS_LAST_CORRUPTED
         filedate=` getCurDate "check_integrity" `
         echo ${filedate} > $_FILE_STATUS_LAST_CORRUPTED

         logprint "${DB_SERVER_DESC} server failed to start. Attempting to recover..."

         check_createdbSchedule
         if [ "x$?" = "x0" ]; then
             logprint "WARNING: Previous Restore database failed (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ})."
             create_history $_FILE_STATUS_LAST_CORRUPTED "WARNING: Previous Restore database failed (REASON=${CREATE_DB_SRC} FILE=${CREATE_DB_SRC_TGZ})."
         fi              

         create_history $_FILE_STATUS_LAST_CORRUPTED "DATABASE CORRUPTED"

         #this function is called after populate_database. Hence no need to check any prev schedule exists.
         #check default method to restore from factory_default or restore_snapshot
         if [ -f $_FILE_FLAG_RESTORE_FACTORY ] ; then
             #Try reset
             reset "restore_factory" "true"
         else
             #Try restore
             schedule_restoreSnapshot "restore_lastbackup" "true"
             #schedule_restoreSnapshot can fail- if no snapshots or invalid snapshot file..
             if [ "x$?" != "x0" ]; then
                 reset "restore_factory" "true"
             fi
         fi
         #restart needed 
         return 1
     fi
     
    #from the time the db was created....
    #upgrade if backupAppVersion is bumped up/DB_SERVER changed ... 
     if [ ! -f $_FILE_INFO_LASTRESTORED_DB ]; then
         logprint "WARNING: previous backup information is not available, defaulting to DBServer  ${DB_SERVER} backupAppVersion 1 serviceVersion 1.4 " 
         createBackupInfo $_FILE_INFO_LASTRESTORED_DB "1"
     else
         local upgrade_progress=`findPropertyInFile_withDefault $_FILE_INFO_LASTRESTORED_DB upgrade_progress "no"`
         #logprint " ----- upgrade_progress ----- -$upgrade_progress=?no"
         if [ "$upgrade_progress" = "no" ]; then
             #in case of downgrade and upgrade.. maybe some left over file. first support  started at serviceVersion 2.20
             local isScol=`native_isSCol`
             local backupDBServer=`findPropertyInFile_withDefault $_FILE_INFO_LASTRESTORED_DB DBServer "${DB_SERVER}"`
             #logprint " ----- upgrade: isScol ----- -$isScol=?true"
             if [ "$isScol" = "true" -a "${DB_SERVER}" = "exist_xmldb" ]; then
                 #logprint " 2----- upgrade: isScol ----- -$isScol=?true"
                 logprint "WARNING: Single collection found, maybe downgraded and upgraded, defaulting to DBServer  ${DB_SERVER} backupAppVersion 1 serviceVersion 1.4 " 
                 /bin/rm -rf $_FILE_INFO_LASTRESTORED_DB
                 createBackupInfo $_FILE_INFO_LASTRESTORED_DB "1"
             fi
         fi
     fi

     local backupAppVersion=`findPropertyInFile_withDefault $_FILE_INFO_LASTRESTORED_DB backupAppVersion 1`
     local serviceVersion=`findPropertyInFile_withDefault $_FILE_INFO_LASTRESTORED_DB serviceVersion 1.4`
     local backupDBServer=`findPropertyInFile_withDefault $_FILE_INFO_LASTRESTORED_DB DBServer "${DB_SERVER}"`
     local backupAppVersion=`expr $backupAppVersion + 0`	
     if [ $backupAppVersion -eq $BACKUP_APP_VERSION -a "$backupDBServer" = "$DB_SERVER" ]; then
         #logprint "upgrade db is not needed $backupAppVersion -eq $BACKUP_APP_VERSION -a  '$backupDBServer' = '$DB_SERVER' " 
         #logprint "upgrade db is not needed" 
         return 0	
     fi
     logprint "upgrade_db from ${backupAppVersion}:${serviceVersion}:${backupDBServer} to ${DB_SERVER}" 
     local upgrade_desc="upgrade_${backupAppVersion}_to_${BACKUP_APP_VERSION}"
     create_history $_FILE_STATUS_LAST_CORRUPTED "DATABASE ${upgrade_desc} upgrade_db backupAppVersion=${backupAppVersion} serviceVersion=${serviceVersion} backupDBServer=${backupDBServer} DBServer=${DB_SERVER}"
     
     if [ $backupAppVersion -gt $BACKUP_APP_VERSION ] ; then
         logprint "WARNING: upgrade_db ?? $backupAppVersion -lt $BACKUP_APP_VERSION ..... in future.. reseting " 
        #Try reset
        reset "restore_factory_${upgrade_desc}" "true"  
     else
         logprint "upgrade_db ?? $backupAppVersion -lt $BACKUP_APP_VERSION ..... in past.." 
         if [ "$backupDBServer" = "$DB_SERVER" ] ; then
        	if [ -f $_FORCE_RESET_FILE ]; then
                  reset "restore_factory_${upgrade_desc}" "true"
                  rm -rf $_FORCE_RESET_FILE
    		 else 
	             logprint "Upgrading db : Source and destination database is same, create snapshot and restore." 
	             create_snapshot_internal  "restore_lastbackup_${upgrade_desc}"
	             schedule_restoreSnapshot "restore_lastbackup_${upgrade_desc}" "true"
	         #schedule_restoreSnapshot can fail- if no snapshots or invalid snapshot file..
	             if [ "x$?" != "x0" ]; then
	                 reset "reset_${upgrade_desc}" "true"
	             fi
        	fi
         else
             logprint "Upgrading db : Source and destination database is not same, based on settings- restore from from snapshot or factory." 
         #if db is not same then reset/restore
             if [ -f $_FILE_FLAG_RESTORE_FACTORY ] ; then
             #Try reset
                 reset "restore_factory_${upgrade_desc}" "true"
             else
             #Try restore
                 schedule_restoreSnapshot "restore_lastbackup_${upgrade_desc}" "true"
             #schedule_restoreSnapshot can fail- if no snapshots or invalid snapshot file..
                 if [ "x$?" != "x0" ]; then
                     reset "restore_factory_${upgrade_desc}" "true"
                 fi
             fi
         fi    
     fi

     #restart needed 
     return 1
 }


 # "Start" command
 start_implementation() {

     logshow "Starting ${DB_SERVER_DESC} service."
     
     start_database

     # see if the db is ok... if not in proper state.. try to recover
     check_integrity
     if [ "x$?" != "x0" ]; then
         logshow "${DB_SERVER_DESC} check_integrity : stopping stop_database."
         stop_database
         sleep 3
         logshow "${DB_SERVER_DESC} check_integrity : starting start_database."
         start_database

         check_integrity
         if [ "x$?" != "x0" ]; then
             logshow "${DB_SERVER_DESC} check_integrity : failed to start."
             create_history $_FILE_INFO_LASTRESTORED_DB "DATABASE START FAILED"
             return 1
         fi

     fi
     logshow "${DB_SERVER_DESC} server is started."

     create_history $_FILE_INFO_LASTRESTORED_DB "DATABASE STARTED"

     return 0
 }

 # "Stop" command
 stop_implementation() {

     logshow "Stopping ${DB_SERVER_DESC} Server."
     stop_database
     if [ "x$?" != "x0" ]; then
         logprint "Waring: Attempting to stop ${DB_SERVER_DESC} server failed."
     else
         logprint "${DB_SERVER_DESC} server is stopped."
     fi

     return 0
 }

 ####### saparated from start_implementation stop_implementation #############
# stop database 
stop_database() {
     native_XMLDB_Stop
     return $?
}

# start database 
start_database() {
	
	 #find out db data folder is empty then force to restore from factory default 
	local DIR="/data/existdbserver/data/*"
	if [ ! "$(ls -A $DIR)" ]; then
    	touch ${_FORCE_RESET_FILE}
	fi
	
     # Call native for sedna_native/sedna_xmldb/exist_xmldb	
     native_stopIfRunning
    
    #Consistency check
	run_exist_consistency_check
    if [ "x$?" != "x0" ]; then
       	#consistency check failed
		delete_index_files	
	fi

     # Check if we need to delete existing database
     cleanup_database

     # Start se_gov
     native_XMLDB_Start

     # Create empty db if needed
     check_database

     # Start se_sm
     native_XMLDB_SM_Start

     # Populate database if needed
     populate_database
		 
     return 0
}
####### saparated from start_implementation stop_implementation #############
 
 restore_snapshot(){
     check_lock || return 1

     #schedudle and re_start. 
     schedule_restoreSnapshot "$1"
     if [ "x$?" != "x0" ]; then
         logprint "ERROR: Failed trying Restore Snapshot."
         release_lock
         return 1
     fi

     stop_implementation
     sleep 3

     start_implementation
     sleep 3

     release_lock
     return 0
 }

 stop_replication() {

    #retry the stop until it succeeds 
    while(true); do
            stop
            if [ "x$?" = "x0" ]; then
	            logprint "INFO:database stopped"
                break
            fi
            
            if [ $counter = 60 ]; then             
                logprint "ERROR:database stop failed"
                break
            fi 
            counter = 'expr ${counter} + 1'
            sleep 2
     done

     # miyer: sometimes the native stop does not stop the db, need to debug why
     pids=`ps -o pid,rss,args  | grep /usr/local/exist/start.jar | grep jetty | cut -f2 -d' '`
     if [ ! -z $pids ] ; then
         logprint "WARNING:StopReplication postprocessing server still running  ${pids} ... kill -9 "
         echo -e "\nWARNING:StopReplication postprocessing server still running  ${pids} ... kill -9 " >> $_FILE_REPLICATION_STATUS
         kill -9 $pids
     fi     

     time_stamp=`date +%d-%H:%M:%S`
     echo -e "\n${time_stamp} StopReplication.\n" >> $_FILE_REPLICATION_STATUS

     logprint "INFO: StopReplication postprocessing:Remove Journal & Lock Files."
     rm -rf /data/existdbserver/data/journal; 
     rm -rf /data/existdbserver/data/*.lck; 
     return 0
 }


 validate_input() {
     dir=${1}     
     dir_len=`expr length "$dir"`
     fileName=${2}
     fileName_len=`expr length "$fileName"`
    
     if [ "x$dir_len" = "x0"  -o  "x$fileName_len" = "x0" ]; then
         logprint "INFO: Dir path and File name required."
         exit 1
     fi
 }

 check_replication_status() {     

     logprint "INFO: checking isDBDateAccessible ."
     # check if we can read th edate
     native_isDBDateAccessible
     if [ "x$?" != "x0" ]; then
         time_stamp=`date +%d-%H:%M:%S`
         echo -e "\n${time_stamp} StartReplicationFailedNoDate.\n" >> $_FILE_REPLICATION_STATUS
         logprint "INFO: StartReplicationFailed DBDate not accessible."
         return 1
     fi 

     logprint "INFO: checking for data-page error ."
     local failed="`grep "not a data-page" $_FOLDER_IPVSDB_SERVER_LOGDIR/exist.log | wc -c`"
     failed=`expr $failed + 0`
     if [ $failed -gt 0 ] ; then
        time_stamp=`date +%d-%H:%M:%S`
        echo -e "\n${time_stamp} StartReplicationFailedDataPage.\n" >> $_FILE_REPLICATION_STATUS
        logprint "INFO: StartReplicationFailed data-page error."
        return 1
     fi

 }

 start_replication() {
     
     validate_input ${1} ${2}
     
     download_dir=${1}
     fileName=${2}      

     if [ -d ${download_dir}/data ]; then
        logprint "INFO: Remove data directory before untaring from working area. ${download_dir}/data"
    	rm -rf ${download_dir}/data
     fi 

     logprint "INFO Untar downloaded tgz to working area ${download_dir}"
     cd ${download_dir}; tar xvf ${fileName}

     if [ "x$?" != "x0" ]; then
       logprint "INFO: Untar of downloaded tgz file failed."   
       rm -rf ${download_dir}/data  
       time_stamp=`date +%d-%H:%M:%S`
       echo -e "\n${time_stamp} UnTarFailed\n" >> $_FILE_REPLICATION_STATUS       
       exit 1
     fi 

     #TBD: Put some validations to make sure the tar is good

     logprint "INFO Back up existing Database. ${_FOLDER_IPVSDB_SERVER_DATA}/data to ${_FOLDER_IPVSDB_SERVER_DATA}/data_save"
     mv ${_FOLDER_IPVSDB_SERVER_DATA}/data ${_FOLDER_IPVSDB_SERVER_DATA}/data_save

     logprint "INFO: Copy downloaded database to live db "
     mv ${download_dir}/data ${_FOLDER_IPVSDB_SERVER_DATA}   

     if [ "x$?" != "x0" ]; then
       logprint "INFO: Copying data base failed. ${_FOLDER_IPVSDB_SERVER_DATA}/data" 
       time_stamp=`date +%d-%H:%M:%S`
       echo -e "\n${time_stamp} CopyDataBaseFailed\n" >> $_FILE_REPLICATION_STATUS           

       mv ${_FOLDER_IPVSDB_SERVER_DATA}/data_save ${_FOLDER_IPVSDB_SERVER_DATA}/data        
       logprint "INFO: Restore database. ${_FOLDER_IPVSDB_SERVER_DATA}/data_save to ${_FOLDER_IPVSDB_SERVER_DATA}/data" 
       exit 1
     fi   

     start
    
     check_replication_status
 
     if [ "x$?" != "x0" ]; then
       mv ${_FOLDER_IPVSDB_SERVER_DATA}/data_save ${_FOLDER_IPVSDB_SERVER_DATA}/data        
       logprint "INFO: Restore database. ${_FOLDER_IPVSDB_SERVER_DATA}/data_save to ${_FOLDER_IPVSDB_SERVER_DATA}/data_save" 

       time_stamp=`date +%d-%H:%M:%S`
       echo -e "\n${time_stamp} StartReplicationFailed.\n" >> $_FILE_REPLICATION_STATUS       
       exit 1
     fi 
    
     rm -r ${_FOLDER_IPVSDB_SERVER_DATA}/data_save
     time_stamp=`date +%d-%H:%M:%S`
     echo -e "\n${time_stamp} StartReplication.\n" >> $_FILE_REPLICATION_STATUS
     logprint "INFO: StartedReplication."
 }


 create_replication_file() {

     validate_input ${1} ${2}
     
     uploadDir=${1}

     logprint "INFO: Checking for Upload Dir ${uploadDir}."
     
     if [ ! -d $uploadDir ]; then
        logprint "INFO: Create Upload Dir ${uploadDir}."
    	mkdir $uploadDir
     fi      
    
      
     archivefileName="${uploadDir}/${2}"

     if [  -f $archivefileName ]; then
        logprint "INFO: remove dowload file ${archivefileName}."
    	rm $archivefileName
     fi 

     logprint "INFO: archivefileName ${archivefileName}."      
     
    cd ${_FOLDER_IPVSDB_SERVER_DATA}; tar czvf ${archivefileName} data --exclude=data/sanity --exclude=data/*.lck  

    if [ "x$?" != "x0" ]; then
       logprint "INFO: Creation of Replication tgz file failed."         
       exit 1
    fi 
      
     logprint "INFO: Created Replication tgz file."
     
 }
 
 
 
 clean_journal() {
     
     journaldir=/data/existdbserver/data/journal

     #logprint "INFO: Checking for Journal Dir ${journaldir}."
     
     if [ ! -d $journaldir ]; then
        logprint "INFO: no journal directory ignore gc request."
    	exit 1
     fi      
    
     `rm -rf /data/existdbserver/journalfiles`
     
     ret_status=`ls ${journaldir}/*.jbx -rt | grep -v jei  > /data/existdbserver/journalfiles`
     
     count=`cat /data/existdbserver/journalfiles | wc -l` 
     
     if [ $count -gt 2 ]; then
        logprint "INFO: journal files ${count}." 
     fi
     
     count=`expr $count - 2`
          
     if [ $count -gt 0 ]; then                
      	 head -n $count /data/existdbserver/journalfiles | xargs rm
      	 logprint "INFO: Deleting journal files ${count}." 
     fi      
 }
 

delete_index_files(){
logshow "deleting index files"
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/*.log
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/*.lck
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/elements.dbx
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/words.dbx
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/values.dbx
rm -f ${_FOLDER_IPVSDB_SERVER_MSDB}/ngram.dbx
local file_name=${CONSISTENCY_CHECK_LOG_FILE}
touch ${file_name}
local filedate=`date +%c%t`
echo "${filedate} : Index files deleted after consistency check failed" >> ${file_name} 
}

 ################## start-stop and functions using start - stop  re_start , import ,  restore ###################

 # Is DATABASE Server installed??
 native_isDatabaseInstallationValid
 if [ "x$?" != "x0" ]; then
     exit 1
 fi 

 action_name="$1"
 HELP_ADDITONAL_CMDS="|schedule_restoreSnapshot|schedule_importdb|restore_snapshot|import|create_snapshot|export|export_snapshot|verify_database_tar"
 case "$1" in
     
     create_snapshot)
     #create a new last snapshot with discription to be shown in menu
     #create_snapshot  "REASON=any discription"
     create_snapshot "$2"
     ;;

     restore_snapshot)
     #restores last snapshot
     #restore_snapshot "REASON=lastbackup/restore_lastbackup"  "WAS_CORRUPTED=true/false"
     restore_snapshot "$2"
     ;;

     export_snapshot)
     #copies last snapshot to a given file name in /data/saved_dabases/FILE_NAME.tgz or to FILE_PATH
     #export_snapshot "FILE_NAME/FILE_PATH" "if second argument not empty.. FILE_NAME is fullpath."
     export_snapshot "$2" "$3" "$4"
     ;;

     schedule_restoreSnapshot)
     #restores last snapshot on next reboot
     #schedule_restoreSnapshot "REASON=lastbackup/restore_lastbackup" "WAS_CORRUPTED=true/false"
     schedule_restoreSnapshot "$2"  
     ;;


     export)
     #create a snap shot and copies the file to /data/saved_dabases/FILE_NAME.tgz.  With REASON as discription to be shown in menu.
     # exportdb "FILE" "REASON=any discription"
     exportdb "$2" "$3" "$4"
     ;;


     schedule_importdb)
     #imports given db on enxt reboot    
     # schedule_importdb "FILE"
     schedule_importdb "$2" 
     ;;

     import)
     #imports given db
     #importdb "FILE"
     importdb "$2" 
     ;;


     verify_database_tar)
     #verifies given tar is valid or not    
     #verify_database_tar "FILE_PATH"
     verify_database_tar "$2"
     ;;

     stopreplication)
     stop_replication;
     ;;


     startreplication)
     start_replication "$2" "$3"
     ;;
     
     cleanjournal)
     clean_journal
     ;;

     createreplicationfile)
     #create tar file of exist data folder in given folder
     create_replication_file "$2" "$3"
     ;;
     
     check_createdbScheduled)
     check_createdbSchedule
     ;;
     *) handle_command $1

 esac

 

