#!/bin/sh
WATCHABLE="false"
SCRIPT_PREFIX="/etc/scripts/watchdog"
#SCRIPT_PREFIX="/home/miyer/worksvn/mediaserver/system"
SERVER_NAME="OSGI"
SERVER_CTL_FILE_PREFIX="osgi"
SERVER_STATUS_FILE_PREFIX="osgiserver"
THIS_SCRIPT_PATH="/etc/scripts/osgictl.sh"

OSGIPRODUCT="/home/onair/modules/mediaroom/osgi"
OSGILOGDIR="/var/log/osgi"
OSGILICENSEDIR="/home/onair/modules/mediaroom/osgi/license"
STATUS_READY_FILE="/var/status/appservice.ready" 
source "${SCRIPT_PREFIX}/cmdctlbase.sh"
JAVATEMPDIR="/data/javaiotmp/"

if [ ! -f /etc/config/arm_system ] ; then
    TOTALSYSMEM=`cat /proc/meminfo | grep MemTotal |  sed -r  's/^[^0-9]*([0-9]+).*/\1/'`
fi


JAVAMEM=-Xmx512m
if [ $TOTALSYSMEM -gt 7000000 ] ; then

    JAVAMEM=-Xmx2048m

fi
GC_OPTS="-XX:+UseG1GC -XX:MinHeapFreeRatio=10 -XX:MaxHeapFreeRatio=25"
start_command() {
    mkdir -p ${OSGILOGDIR}
    chmod 777 ${OSGILOGDIR}
    mkdir -p ${OSGILICENSEDIR}
    chmod 777 ${OSGILICENSEDIR}
    export BLM_DYNAMIC_DIR=${OSGILICENSEDIR}
    export BLM_LICENSE_FILE=${OSGILICENSEDIR}/nimbus.lic
    /bin/rm -rf $STATUS_READY_FILE
	
    JAVA=java
${JAVA} ${JAVAMEM} ${GC_OPTS} -Dosgi.clean=true -Djava.io.tmpdir=${JAVATEMPDIR} -Declipse.ingnoreApp=true -Dosgi.noShutdown=true  -Dorg.osgi.service.http.port=10087  -Dorg.eclipse.equinox.http.jetty.autostart=true  -Dbundles.configuration.location=${OSGIPRODUCT}/configadmin  -jar ${OSGIPRODUCT}/plugins/org.eclipse.equinox.launcher_1.3.0.v20130327-1440.jar  >> ${OSGILOGDIR}/mrapp.log &

    PID=$!
    #echo "start_command PID=${PID}"
     # Wait for OSGI App Service to be Ready to accept messages. MSComponent will write this file.
    local err=0
    count=0
    timout=120
    while(true); do
     count=`expr ${count} + 1`
     if [ -f $STATUS_READY_FILE ]; then
        break
     fi
     if [ $count -gt $timout ]; then
	     err=1
	     break
     fi
     logprint "Waiting to start ${SERVER_NAME} and ready. [${count}/${timout}]"
     sleep 1
    done
    return 0
}

handle_command $1
