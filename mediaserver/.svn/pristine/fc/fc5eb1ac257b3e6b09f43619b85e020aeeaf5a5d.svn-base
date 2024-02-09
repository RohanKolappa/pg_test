#!/bin/sh
#
# openfirectl	Stops and starts the Openfire XMPP service.
#
# chkconfig: 2345 99 1
# description: Openfire is an XMPP server, which is a server that facilitates \
#              XML based communication, such as chat.
# config: /opt/openfire/conf/openfire.xml
# config: /etc/sysconfig/openfire
# pidfile: /var/run/openfire.pid
#
# This script manages the openfire daemon process.  This script should
# only be run as root and may double as an init script if desired.  You
# can symlink it into your rc directories manually, or if you have
# chkconfig available, you can copy/symlink it into your init.d directory
# and chkconfig --add it.  It's recommended to name it openfire instead of
# openfirectl in the init.d directory.
#

#####
# Begin setup work
#####

STATUS_INITIALIZING_FILE="/var/status/openfire.java.initializing"
# Initialization
CMD=$1
PATH="/sbin:/bin:/usr/bin:/usr/sbin"
RETVAL=0

OPENFIRE_USER=root                    
OPENFIRE_HOME=/home/onair/openfire
OPENFIRE_LOGDIR=/var/log/openfire



logprint() {
    logger "$1"
    echo "$1"
}

logshow() {
    echo "$1"
}

# Make sure we are running as root
set `/usr/bin/id`
if [ $1 != "uid=0(root)" ]; then
    echo "$0: must be run as root"
    exit 1
fi

# Get config from sysconfig if on a system that supports it.
[ -f "/etc/sysconfig/openfire" ] && . /etc/sysconfig/openfire

# If openfire user is not set elsewhere, set to daemon.
[ -z "$OPENFIRE_USER" ] && OPENFIRE_USER="daemon"

# If pid file path is not set elsewhere, set to /var/run/openfire.pid.
[ -z "$OPENFIRE_PIDFILE" ] && OPENFIRE_PIDFILE="/var/run/openfire.pid"

# -----------------------------------------------------------------

# If a openfire home variable has not been specified, try to determine it.
if [ -z "$OPENFIRE_HOME" -o ! -d "$OPENFIRE_HOME" ]; then
    if [ -d "/usr/share/openfire" ]; then
        OPENFIRE_HOME="/usr/share/openfire"
    elif [ -d "/usr/local/openfire" ]; then
        OPENFIRE_HOME="/usr/local/openfire"
    elif [ -d "/opt/openfire" ]; then
        OPENFIRE_HOME="/opt/openfire"
    elif [ -d "/home/onair/openfire" ]; then
        OPENFIRE_HOME="/home/onair/openfire"
    else
        echo "Could not find Openfire installation under /opt, /usr/share, or /usr/local."
        if [ -f "/etc/sysconfig" ]; then
            echo "Please specify the Openfire installation location as variable OPENFIRE_HOME"
            echo "in /etc/sysconfig/openfire."
        fi
        exit 1
    fi
fi

# If log path is not set already, set to $OPENFIRE_HOME/logs.
[ -z "$OPENFIRE_LOGDIR" ] && OPENFIRE_LOGDIR="${OPENFIRE_HOME}/logs"

# Attempt to locate java installation.
JAVA_HOME="/usr"
JAVACMD="${JAVA_HOME}/bin/java"
JAVATEMPDIR="/data/javaiotmp/"
if [ ! -d "$JAVA_HOME" -o ! -x "$JAVACMD" ]; then
    echo "Error: JAVA_HOME is not defined correctly."
    echo "       Can not sure execute $JAVACMD."
    exit 1
fi

# Prepare location of openfire libraries
OPENFIRE_LIB="${OPENFIRE_HOME}/lib"

# Increase Memory for openfire...-Xmx128m

if [ ! -f /etc/config/arm_system ] ; then
    TOTALSYSMEM=`cat /proc/meminfo | grep MemTotal |  sed -r  's/^[^0-9]*([0-9]+).*/\1/'`
fi 
OPENFIRE_OPTS="-Xmx256m"
if [ $TOTALSYSMEM -gt 7000000 ] ; then

    OPENFIRE_OPTS="-Xmx512m"
fi
GC_OPTS="-XX:+UseG1GC -XX:MinHeapFreeRatio=10 -XX:MaxHeapFreeRatio=25"

# http://jira.codehaus.org/browse/JETTY-331 . Sun bug numbers: 6202721, 6521844, 5031872.
# openfire hangs during startup. jdk bug in SecureRandom(java.security.SecureRandom.next(SecureRandom.java:455)). 
# SecureRandom being invoked from jetyy web server(org.mortbay.jetty.servlet.HashSessionIdManager.doStart(HashSessionIdManager.java:105)
# FIX: add JVM argument -Djava.security.egd=file:/dev/./urandom 

# Prepare openfire command line
OPENFIRE_OPTS="${OPENFIRE_OPTS} ${GC_OPTS} -Djava.io.tmpdir=${JAVATEMPDIR} -DopenfireHome=${OPENFIRE_HOME} -Dopenfire.lib.dir=${OPENFIRE_LIB} -Djava.security.egd=file:/dev/./urandom "

# Prepare local java class path
if [ -z "$LOCALCLASSPATH" ]; then
    LOCALCLASSPATH="${OPENFIRE_LIB}/startup.jar"
else
    LOCALCLASSPATH="${OPENFIRE_LIB}/startup.jar:${LOCALCLASSPATH}"
fi

# Export any necessary variables
export JAVA_HOME JAVACMD

# Lastly, prepare the full command that we are going to run.
OPENFIRE_RUN_CMD="${JAVACMD} -server ${OPENFIRE_OPTS} -classpath \"${LOCALCLASSPATH}\" -jar \"${OPENFIRE_LIB}/startup.jar\""

#####
# End setup work
#####


# Check Openfire database files exist and create database if needed
check_database() {

    if [  ! -f /data/xmppserver/conf/openfire.xml ] ; then

        logprint "Creating Openfire factory database files..."

        /bin/cp /home/onair/templates/openfire.xml.localdb /data/xmppserver/conf/openfire.xml
        /bin/cp /home/onair/templates/openfire.script.localdb /data/xmppserver/embedded-db/openfire.script
        /bin/cp /home/onair/templates/openfire.properties.localdb /data/xmppserver/embedded-db/openfire.properties
    fi

    if [  ! -f /data/xmppserver/resources/security/keystore ] ; then

        logprint "Creating Openfire factory security files..."

        /bin/cp /home/onair/templates/openfire.keystore /data/xmppserver/resources/security/keystore
        /bin/cp /home/onair/templates/openfire.client.truststore /data/xmppserver/resources/security/client.truststore
        /bin/cp /home/onair/templates/openfire.truststore /data/xmppserver/resources/security/truststore
    fi

}


# Delete Openfire database files if needed
cleanup_database() {
    
    if [ -f /data/xmppserver/delete_database ] ; then

        rm -f /data/xmppserver/delete_database

        logprint "Deleting Openfire database files..."

        /bin/rm -f /data/xmppserver/conf/openfire.xml
        /bin/rm -rf /data/xmppserver/embedded-db/*

        /bin/rm -f /data/xmppserver/resources/security/*

    fi

}

# "Reset" command
reset() {
    # Touch a special file that would signal a database re-create at
    # next Openfire start
    /bin/touch /data/xmppserver/delete_database

}


start() {
    OLD_PWD=`pwd`
    cd $OPENFIRE_LOGDIR

    # Start daemons.
    logprint "Starting Openfire..."

    # Is Openfire already running??
    if [ -f "$OPENFIRE_PIDFILE" ] ; then
        pid=`cat $OPENFIRE_PIDFILE 2>&1`
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            logprint "Openfire already running!!"
            return 1
        else
            logprint "Openfire should have been running???"
            rm -f "$OPENFIRE_PIDFILE"
        fi
    fi

    # Check if we need to delete existing database
    cleanup_database

    # Create database if needed
    check_database

    #	PID=`su - $OPENFIRE_USER -c "nohup $OPENFIRE_RUN_CMD > $OPENFIRE_LOGDIR/nohup.out 2>&1 &"`
    nohup ${JAVACMD} -server ${OPENFIRE_OPTS} -classpath ${LOCALCLASSPATH} -jar ${OPENFIRE_LIB}/startup.jar &
    PID=$!
    RETVAL=$?
    if [ $RETVAL -eq 0 -a ! -z "$OPENFIRE_PIDFILE" ]; then
        echo $PID > $OPENFIRE_PIDFILE
    fi
    
    echo $PID > $STATUS_INITIALIZING_FILE

    local err=0
    local count=0
    local timout=120
    while(true); do
        count=`expr ${count} + 1`
        if [ ! -f $STATUS_INITIALIZING_FILE ]; then
            break
        fi
        if [ $count -gt $timout ]; then
            err=1
            break
        fi
        logprint "Waiting to start Openfire. [${count}/${timout}]"
        sleep 1
    done
    
    if [ $err = "1" ] ; then
        logprint "WARNING: Could not start Openfire !"
    fi

    #double check 
    while(true); do
        count=`expr ${count} + 1`
        /bin/netstat -an | grep 10090 | grep LISTEN > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
            break
        fi
        if [ $count -gt $timout ]; then
            err=1
            break
        fi
        logprint "Waiting to start Openfire and Ready. [${count}/${timout}]"
        sleep 1
    done

    if [ $err = "1" ] ; then
        logprint "WARNING: Could not start Openfire."
        cd $OLD_PWD
		#return 1
    fi
    
    echo
    [ $RETVAL -eq 0 -a -d /var/lock/subsys ] && touch /var/lock/subsys/openfire

    logprint "Openfire Started"
    sleep 1 # allows prompt to return
    cd $OLD_PWD


    logprint "Creating links for client download"
	/bin/ln -sf /home/onair/modules/clients/public /home/onair/htdocs/public/clients
    return 0

}

stop() {
    if [ ! -f "$OPENFIRE_PIDFILE" ] ; then
        logprint "Openfire already stopped !!"
        return 1
    fi

    # Stop daemons.
    logprint "Stopping Openfire..."
    # Kill Openfire
    pid=`cat $OPENFIRE_PIDFILE 2>&1`
    kill $pid

    # Confirm Openfire has really stopped (wait for 10 sec)
    local err=0
    local count=0
    local timout=90
    while(true); do
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" != "x0" ]; then
            break
        fi
        count=`expr ${count} + 1`
        if [ $count -gt $timout ]; then
            err=1
            break
        fi
        logprint "Waiting to stop Openfire. [${count}/${timout}]"
        sleep 1
    done

    if [ $err = "1" ] ; then
	#use kill -9 
	logprint "ERROR: Could not stop Openfire. Using kill -9 !!"
	kill -9 $pid > /dev/null 2>&1
	err=0
        count=0
        timout=90
        while(true); do
            kill -0 $pid > /dev/null 2>&1
            if [ "x$?" != "x0" ]; then
                break
            fi
            count=`expr ${count} + 1`
            if [ $count -gt $timout ]; then
                err=1
                break
            fi
            logprint "Waiting for Openfire to really die. [${count}/${timout}]"
            sleep 1
        done

    fi


    [ -f "$OPENFIRE_PIDFILE" ] && rm -f $OPENFIRE_PIDFILE 
    [ -f "/var/lock/subsys/openfire" ] && rm -f /var/lock/subsys/openfire

    if [ $err = "1" ] ; then
        logprint "ERROR: Could not stop Openfire."
        return 1
    fi

    logprint "Openfire Stopped"
    logprint "Removing links for client download"
	/bin/rm -f /home/onair/htdocs/public/clients
    return 0
}

restart() {
    stop
    sleep 10 # give it a few moments to shut down
    start
}

condrestart() {
    [ -e "/var/lock/subsys/openfire" ] && restart
    return 0
}

status_old() {
    pid=`cat $OPENFIRE_PIDFILE 2>&1`
    if [ x"$?" = "x1" ]; then
        echo "Openfire is Stopped"
        RETVAL=0
    else 
        kill -0 $pid > /dev/null 2>&1
        if [ x"$?" = "x0" ]; then 
            /bin/netstat -an | grep 10090 | grep LISTEN > /dev/null 2>&1
            if [ x"$?" = "x0" ]; then 
                echo "Openfire is Ready"
            else
                echo "Openfire is Running"
            fi
            RETVAL=0
        else 
            echo "Openfire is Stopped"
            RETVAL=0
        fi
    fi
}


status() {
    if [ ! -f $OPENFIRE_PIDFILE ]; then
        echo "NOT_STARTED"
        RETVAL=1
        return
    fi

    pid=`cat $OPENFIRE_PIDFILE 2>&1`
    if [ x"$?" = "x1" ]; then
        echo "FAILED_TO_START"
        RETVAL=3
    else 
        kill -0 $pid > /dev/null 2>&1
        if [ x"$?" = "x0" ]; then 
            /bin/netstat -an | grep 10090 | grep LISTEN > /dev/null 2>&1
            if [ x"$?" = "x0" ]; then 
                echo "OK"
                RETVAL=0
            else
                echo "STARTING"
                RETVAL=2
            fi
        else 
            echo "DEAD"
            RETVAL=3
        fi
    fi
}


# Handle how we were called.
case "$CMD" in
    start)
    start
    ;;
    stop)
    stop
    ;;
    restart)
    restart
    ;;
    condrestart)
    condrestart
    ;;
    reload)
    restart
    ;;
    status) 
    status
    ;;
    reset)
    reset
    ;;
    *)
    echo "Usage $0 {start|stop|restart|status|condrestart|reload|reset}"
    RETVAL=1
esac

exit $RETVAL
