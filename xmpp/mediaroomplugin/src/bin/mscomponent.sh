#!/bin/sh
. /etc/scripts/database_management_variables.rc
logprint() {
	 logger "$1"
	 echo "$1"
}

RETVAL=0
ARGUMENTS=""

IPVSCOMPONENT_HOME=/home/onair/ipvscomponent
MSCOMPONENT_APP_NAME=mscomponent
MSCOMPONENT_HOME=/home/onair/mscomponent
MSCOMPONENT_LOGDIR=/var/log/${MSCOMPONENT_APP_NAME}
MSCOMPONENT_CONFIG=${MSCOMPONENT_HOME}/conf/${MSCOMPONENT_APP_NAME}/${MSCOMPONENT_APP_NAME}.xml
MSCOMPONENT_LOG4J_CONFIG=${MSCOMPONENT_HOME}/conf/${MSCOMPONENT_APP_NAME}/log4j.xml

_FILE_LOCK_INSTANCE="/tmp/lock_${MSCOMPONENT_APP_NAME}.instance" 
MSCOMPONENT_PIDFILE="/var/run/${MSCOMPONENT_APP_NAME}.pid"
STATUS_INITIALIZING_FILE="/var/status/${MSCOMPONENT_APP_NAME}.java.initializing"
STATUS_READY_FILE="/var/status/${MSCOMPONENT_APP_NAME}.ready"
IPVSCOMPONENT_LIB="${IPVSCOMPONENT_HOME}/lib"
MS_CLASSPATH=${MSCOMPONENT_HOME}/lib/${MSCOMPONENT_APP_NAME}.jar
MR_CLASSPATH=/home/onair/modules/mediaroom/public

#load database type
. ${IPVSCOMPONENT_HOME}/bin/_db_server.rc

#mscomponent should be started ? enable_mscomponent=true/false
#disable_mscomponent=true ... mscomponent will be not started  
disable_mscomponent=true
MSCOMPONENT_HOME=/home/onair/mscomponent
if [ -f ${MSCOMPONENT_HOME}/bin/_disable_mscomponent.rc ] ; then
	. ${MSCOMPONENT_HOME}/bin/_disable_mscomponent.rc
fi

ENDORSED_LIB="${IPVSCOMPONENT_HOME}/${DB_SERVER}/endorsed"
JAVA_EXEC="/usr/bin/java"
IPVSCOMPONENT_OPTS="-Xmx256m"
if [ $DB_SERVER = "exist_xmldb" ] ; then
	 EXIST_DB_HOME="/usr/local/exist"
	 ENDORSED_LIB="${EXIST_DB_HOME}/lib/endorsed"
	 JAR_NAME=ipvsstarter_exist_xmldb.jar
	 IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} -Djava.endorsed.dirs=${ENDORSED_LIB} -Dexist.home=${EXIST_DB_HOME} "
else
	if [ $DB_SERVER = "sedna_xmldb" -o   $DB_SERVER = "sedna_native" ] ; then
		ENDORSED_LIB="${IPVSCOMPONENT_HOME}/xmldblib/sedna/endorsed"
		JAR_NAME=ipvsstarter_sedna_xmldb.jar
		IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} -Djava.endorsed.dirs=${ENDORSED_LIB}"
	else
		logprint "Unsupported database type '${DB_SERVER}'"
		exit 1
	fi
fi
 

IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} -DipvsAppName=${MSCOMPONENT_APP_NAME} -DipvscomponentHome=${IPVSCOMPONENT_HOME}  -Dipvscomponent.lib.dir=${IPVSCOMPONENT_LIB}  -DipvsConfigFilePath=${MSCOMPONENT_CONFIG}  -Dlog4jConfigFilePath=${MSCOMPONENT_LOG4J_CONFIG}  -Djava.security.egd=file:/dev/./urandom " 

if [ -z "$LOCALCLASSPATH" ]; then
	LOCALCLASSPATH=".:"
else
	LOCALCLASSPATH=".:${LOCALCLASSPATH}"
fi


start() {
	 if [ "x$disable_mscomponent" = "xtrue" ]; then
		 logprint "mscomponent is disabled"
		 return 0
	 fi	
	 # Make sure IPVS Component is not already running
	 if [ -f $MSCOMPONENT_PIDFILE ] ; then
			 pid=`cat $MSCOMPONENT_PIDFILE`
			 kill -0 $pid > /dev/null 2>&1
			 if [ "x$?" = "x0" ]; then
					 logprint "${MSCOMPONENT_APP_NAME} already running... Stop it first."
					 return 1
			 else
					 logprint "${MSCOMPONENT_APP_NAME} should have been running???"
					 rm -f "$MSCOMPONENT_PIDFILE"
			 fi
	 fi

	 OLD_PWD=`pwd`
	 # remove ready status file
	 /bin/rm -rf $STATUS_READY_FILE
	
	 /bin/mkdir -p $MSCOMPONENT_LOGDIR
	 cd $MSCOMPONENT_LOGDIR
	 /bin/rm -rf $STATUS_DB_MIGRATING_FILE
	 # Start daemons.
	 logprint "Starting ${MSCOMPONENT_APP_NAME} for ${DB_SERVER} ..."
	 /bin/cp -f ${MS_CLASSPATH} ${MR_CLASSPATH}
	 CMD_EXEC="nohup ${JAVA_EXEC} -server ${IPVSCOMPONENT_OPTS} -classpath ${LOCALCLASSPATH} -jar ${IPVSCOMPONENT_LIB}/${JAR_NAME} ${ARGUMENTS} &"
	 #printf "${CMD_EXEC} \n   "	
	 eval $CMD_EXEC 
	 PID=$!
	 if [ "x$?" = "x0" ]; then
			 echo $PID > $MSCOMPONENT_PIDFILE
	 fi

	 # create STATUS_INITIALIZING_FILE , MSComponent will remove it on initialization complete.    
	 echo "pid=${PID}" > $STATUS_INITIALIZING_FILE
	 local err=0
	 count=0
	 timout=120
	 while(true); do
			 count=`expr ${count} + 1`
			 if [ ! -f $STATUS_INITIALIZING_FILE ]; then
					 break
			 fi
			 if [ $count -gt $timout ]; then
					 err=1
					 break
			 fi
			 logprint "Waiting to start ${MSCOMPONENT_APP_NAME}. [${count}/${timout}]"
			 sleep 1
	 done
	 
	 if [ $err = "1" ] ; then
			 cd $current_dir
			 logprint "ERROR: Could not start ${MSCOMPONENT_APP_NAME}."
			 return 1
	 fi
	 
	 # Wait for MSComponent Ready to accept messages. MSComponent will write this file.
	 local err=0
	 while(true); do
	 		 count=`expr ${count} + 1`
			 if [ -f $STATUS_READY_FILE ]; then
					 break
			 fi
			 if [ $count -gt $timout ]; then
					 err=1
					 break
			 fi
			 logprint "Waiting to start ${MSCOMPONENT_APP_NAME} and ready. [${count}/${timout}]"
			 sleep 1
	 done
	 
	 if [ $err = "1" ] ; then
			 cd $current_dir
			 logprint "ERROR: Could not start ${MSCOMPONENT_APP_NAME}."
			 return 1
	 fi

	 cd $current_dir

	 logprint "Started ${MSCOMPONENT_APP_NAME} ${DB_SERVER} ..."
	 return 0

}

stop() {
	 if [ "x$disable_mscomponent" = "xtrue" ]; then
		 logprint "mscomponent is disabled"
		 return 0
	 fi	

	 if [ -f $MSCOMPONENT_PIDFILE ] ; then
			 pid=`cat $MSCOMPONENT_PIDFILE`
	 else
			 logprint "${MSCOMPONENT_APP_NAME} not running."
			 return 1
	 fi


	 logprint "Stopping ${MSCOMPONENT_APP_NAME}..."

	 # First do gracefull kill
	 kill $pid  > /dev/null 2>&1
	 local err=0
	 count=0
	 timout=90
	 while(true); do
			 count=`expr ${count} + 1`
			 # IpvsComponent deletes the file $STATUS_JAVA_RUNNIG_FILE on graceful shutdown
			 kill -0 $pid > /dev/null 2>&1
			 if [ "x$?" != "x0" ]; then
					 break
			 fi		
			 if [ $count -gt $timout ]; then
					 err=1
					 break
			 fi
			 logprint "Waiting to stop ${MSCOMPONENT_APP_NAME}. [${count}/${timout}]"
			 sleep 1
	 done

	 # if timed out shoot using -9. something wrong if this happens
	 if [ $err = "1" ] ; then
			 logprint "ERROR: Could not stop ${MSCOMPONENT_APP_NAME}."
			 logprint "Timed out: Using kill -9 !!"
			 kill -9 $pid > /dev/null 2>&1

			 # Finally remove the PID file
			 /bin/rm -f $MSCOMPONENT_PIDFILE
			 return 1
	 fi

	 # Finally remove the PID file
	 /bin/rm -f $MSCOMPONENT_PIDFILE

	 logprint "Stopped ${MSCOMPONENT_APP_NAME}..."
	 return 0
}

status() {
	 if [ "x$disable_mscomponent" = "xtrue" ]; then
		 echo "OK"
		 return 0
	 fi	

	 #check if the IpvsComponent is started 
	 if [ ! -f $MSCOMPONENT_PIDFILE ] ; then
			 echo "NOT_STARTED"
			 return 1
	 else
			 #check if the IpvsComponent process is killed or dead
			 pid=`cat $MSCOMPONENT_PIDFILE`
			 kill -0 $pid > /dev/null 2>&1
			 if [ "x$?" != "x0" ]; then
					 logprint "DEAD"
					 return 2
			 fi
	 fi

	 #check if the IpvsComponent is starting
	 if [  -f $STATUS_INITIALIZING_FILE ] ; then
			 echo "STARTING"
			 return 3
	 fi

	 echo "OK"
	 return 0
}



case "$1" in
	 start)
	 start
	 RETVAL=$?

	 ;;

	 stop)
	 stop
	 RETVAL=$?
	 ;;

	 status)
	 status
	 RETVAL=$?
	 ;;

	 restart)
	 stop
	 sleep 3
	 start
	 RETVAL=$?
	 ;;

	 *)
	 echo "Usage: $0 [start|stop|restart|status]"
	 RETVAL=1

esac
exit $RETVAL
