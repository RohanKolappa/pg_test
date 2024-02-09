#!/bin/sh
#These have to be defined
#------------------------

TESTPREFIX=""
SCRIPT_PREFIX="/etc/scripts/watchdog"

#TESTPREFIX="/tmp/miyer"
#SCRIPT_PREFIX="/home/miyer/worksvn/mediaserver/system"


SERVER_NAME="ClusterServer"
SERVER_CTL_FILE_PREFIX="cluster"
SERVER_STATUS_FILE_PREFIX="clusterserver"
THIS_SCRIPT_PATH="${SCRIPT_PREFIX}/clusterctl.sh"

source "${SCRIPT_PREFIX}/cmdctlbase.sh"

start_command() {
    #/usr/bin/java -cp '/home/miyer/download/jgroup/JGroups-2.2.6.bin/jgroups-all.jar:/home/miyer/download/jgroup/JGroups-2.2.6.bin/commons-logging.jar' org.jgroups.stack.GossipRouter -port 5555 bindaddress 0.0.0.0 &
    /usr/bin/java -cp '/usr/local/exist/lib/core/jgroups-all-2.2.6.jar:/usr/local/exist/lib/core/commons-logging-1.1.1.jar' org.jgroups.stack.GossipRouter -port 5555 bindaddress 0.0.0.0 &
    PID=$!
    #echo "start_command PID=${PID}"
    return 0
}

handle_command $1
