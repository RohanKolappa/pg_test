#!/bin/sh
. /etc/scripts/database_management_variables.rc

#load database type
IPVSCOMPONENT_HOME=/home/onair/ipvscomponent
. ${IPVSCOMPONENT_HOME}/bin/_db_server.rc

logprint() {
	 logger "$1"
	 echo "$1"
}

#IPVSCOMPONENT_APP_NAME=ipvsdbtask sednanativedbtest  existdbtest sednaxmldbtest
#ipvsstarter_sednanativedbtest
RETVAL=0

#db_server=sedna_native/sedna_xmldb/exist_xmldb 
#DB_SERVER is defied in included _db_server.rc 
 
STATUS_JAVA_RUNNIG_FILE="/var/status/ipvscomponent.java.running"
STATUS_INITIALIZING_FILE="/var/status/ipvscomponent.java.initializing"
STATUS_DB_FAILED_FILE="/var/status/ipvscomponent.dbfail"
STATUS_OPENFIRE_FAILED_FILE="/var/status/ipvscomponent.xmppfail"
STATUS_DB_MIGRATING_FILE="/var/status/ipvscomponent.java.db.migrating"

IPVSCOMPONENT_PIDFILE="/var/run/ipvscomponent.pid"
IPVSCOMPONENT_APP_NAME=ipvscomponent
IPVSCOMPONENT_LOGDIR=/var/log/ipvscomponent
IPVSCOMPONENT_LIB="${IPVSCOMPONENT_HOME}/lib"
JAR_NAME=ipvsstarter_${IPVSCOMPONENT_APP_NAME}.jar
JAVA_EXEC="/usr/bin/java"
ENDORSED_LIB="${IPVSCOMPONENT_HOME}/${DB_SERVER}/endorsed"
#IPVSCOMPONENT_CONFIG="${IPVSCOMPONENT_HOME}/conf/ipvscomponent/${DB_SERVER}_${IPVSCOMPONENT_APP_NAME}.xml"
_FILE_LOCK_INSTANCE="/tmp/lock_ipvscomponent.instance" 
JAVATEMPDIR="/data/javaiotmp/"
#IPVSCOMPONENT_OPTS="-Xincgc -Xmx256m"
IPVSCOMPONENT_OPTS="-Xmx256m"
GC_OPTS="-XX:+UseG1GC -XX:MinHeapFreeRatio=10 -XX:MaxHeapFreeRatio=25"
if [ $DB_SERVER = "exist_xmldb" ] ; then
	 EXIST_DB_HOME="/usr/local/exist"
	 #ENDORSED_LIB="${EXIST_DB_HOME}/lib/endorsed"
	 ENDORSED_LIB="${IPVSCOMPONENT_HOME}/xmldblib/exist/endorsed"
	 JAR_NAME=ipvsstarter_exist_xmldb.jar
	 IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} ${GC_OPTS}  -Djava.io.tmpdir=${JAVATEMPDIR} -Djava.endorsed.dirs=${ENDORSED_LIB} -Dexist.home=${EXIST_DB_HOME} "
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
#-DipvsConfigFilePath=${IPVSCOMPONENT_CONFIG}
IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} -DipvsAppName=${IPVSCOMPONENT_APP_NAME} -DipvscomponentHome=${IPVSCOMPONENT_HOME}  -Dipvscomponent.lib.dir=${IPVSCOMPONENT_LIB}   -Djava.security.egd=file:/dev/./urandom " 
if [ -z "$LOCALCLASSPATH" ]; then
	LOCALCLASSPATH=".:"
else
	LOCALCLASSPATH=".:${LOCALCLASSPATH}"
fi


start() {
	 # Make sure IPVS Component is not already running
	 if [ -f $IPVSCOMPONENT_PIDFILE ] ; then
			 pid=`cat $IPVSCOMPONENT_PIDFILE`
			 kill -0 $pid > /dev/null 2>&1
			 if [ "x$?" = "x0" ]; then
					 logprint "IpvsComponent already running... Stop it first."
					 return 1
			 else
					 logprint "IpvsComponent should have been running???"
					 rm -f "$IPVSCOMPONENT_PIDFILE"
			 fi
	 fi

	 OLD_PWD=`pwd`
	 /bin/mkdir -p $IPVSCOMPONENT_LOGDIR
	 cd $IPVSCOMPONENT_LOGDIR
	 /bin/rm -rf $STATUS_DB_MIGRATING_FILE
	 # Start daemons.
	 logprint "Starting IpvsComponent ${DB_SERVER} ..."

	 CMD_EXEC="nohup ${JAVA_EXEC} -server ${IPVSCOMPONENT_OPTS} -classpath ${LOCALCLASSPATH} -jar ${IPVSCOMPONENT_LIB}/${JAR_NAME} ${ARGUMENTS} &"
	 #printf "${CMD_EXEC} \n   "	
	 eval $CMD_EXEC 
	 PID=$!
	 if [ "x$?" = "x0" ]; then
			 echo $PID > $IPVSCOMPONENT_PIDFILE
	 fi

	 # create STATUS_INITIALIZING_FILE , IpvsComponent will remove it on initialization complete.    
	 echo "pid=${PID}" >> $STATUS_INITIALIZING_FILE
	 local err=0
	 count=0
	 timout=90
	 while(true); do
			 count=`expr ${count} + 1`
			 if [ ! -f $STATUS_INITIALIZING_FILE ]; then
					 break
			 fi
			 if [ -f $STATUS_DB_MIGRATING_FILE ]; then
					 break
			 fi
			 if [ $count -gt $timout ]; then
					 err=1
					 break
			 fi
			 logprint "Waiting to start IpvsComponent. [${count}/${timout}]"
			 sleep 1
	 done

	 if [  -f $STATUS_DB_MIGRATING_FILE ]; then
			 err=0
			 count=0
			 timout=450
			 while(true); do
					 count=`expr ${count} + 5`
					 if [ ! -f $STATUS_DB_MIGRATING_FILE ]; then
							 break
					 fi
					 kill -0 $PID > /dev/null 2>&1
					 if [ "x$?" != "x0" ]; then
							 err=1
							 logprint "IpvsComponent database migration process exited abruptly. [${count}/${timout}]"
							 break
					 fi
					 logprint "Waiting to complete  IpvsComponent database migration. [${count}/${timout}]"
					 sleep 5
			 done
	 fi

	 if [ $err = "1" ] ; then
			 cd $current_dir
			 logprint "ERROR: Could not start IpvsComponent."
			 return 1
	 fi

	 cd $current_dir

	 logprint "Started IpvsComponent ${DB_SERVER} ..."
	 return 0

}

stop() {

	 if [ -f $IPVSCOMPONENT_PIDFILE ] ; then
			 pid=`cat $IPVSCOMPONENT_PIDFILE`
	 else
			 logprint "IpvsComponent not running."
			 return 1
	 fi


	 logprint "Stopping IpvsComponent..."

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
			 logprint "Waiting to stop IpvsComponent. [${count}/${timout}]"
			 sleep 1
	 done

	 # if timed out shoot using -9. something wrong if this happens
	 if [ $err = "1" ] ; then
			 logprint "ERROR: Could not stop IpvsComponent."
			 logprint "Timed out: Using kill -9 !!"
			 kill -9 $pid > /dev/null 2>&1

			 # Finally remove the PID file
			 /bin/rm -f $IPVSCOMPONENT_PIDFILE
			 return 1
	 fi

	 # Finally remove the PID file
	 /bin/rm -f $IPVSCOMPONENT_PIDFILE

	 logprint "Stopped IpvsComponent..."
	 return 0
}

status() {

	 #check if the IpvsComponent is started 
	 if [ ! -f $IPVSCOMPONENT_PIDFILE ] ; then
			 echo "NOT_STARTED"
			 return 1
	 else
			 #check if the IpvsComponent process is killed or dead
			 pid=`cat $IPVSCOMPONENT_PIDFILE`
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

	 #check if the IpvsComponent is failed to connect database using native api
	 #need to add other db api as needed... currently only checks sedna native
	 if [  -f $STATUS_DB_FAILED_FILE ] ; then
			 echo "DATABASE_NOT_CONNECTED"
			 return 4
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
