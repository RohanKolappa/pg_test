#!/bin/sh

SCRIPT_PREFIX="/etc/scripts/watchdog"

SERVER_NAME="ManagementServer"
THIS_SCRIPT_PATH="/etc/scripts/watchdog/watch_managementserver.sh"
SERVER_CTL_FILE_PREFIX="ms"
SERVER_STATUS_FILE_PREFIX="mserver"

IPVSXMLDBCTL="/etc/scripts/ipvsxmldbctl.sh"
OPENFIRECTL="/etc/scripts/openfirectl.sh"
IPVSCOMPONENTCTL="/home/onair/ipvscomponent/bin/ipvscomponent.sh"
OSGICTL="/etc/scripts/osgictl.sh"
OPENFIRE_PORT="5222"
IPTABLES=/sbin/iptables


DEBUG_FILE_PATH="/tmp/DEBUGMS"
JAVATEMPDIR="/data/javaiotmp/"

source "${SCRIPT_PREFIX}/ctlbase.sh"

on_starus_down_restart=1

register_sc_service() {
    if [ -f /etc/config/autoregistration.enabled ] ; then
        # Here we should compute the hash of mac address and plug it in
        # Currently using the mac address directly
        local mac_address=`ifconfig eth0 | grep HWaddr | sed -e 's/.*HWaddr \(.*\)/\1/'`
        sed -e "s/HashOfMac/${mac_address}/" /home/onair/templates/sc.service > /etc/avahi/services/sc.service
    fi
}

unregister_sc_service() {
    rm -f /etc/avahi/services/sc.service
}

start_implementation() {

    # Delete any existing stale copies of report work area directories 
    rm -rf /data/reports/WORKAREA*


    # clean java temporary memory and create again
    rm -rf ${JAVATEMPDIR}
    /bin/mkdir -p ${JAVATEMPDIR}
   
    #block 5222 port until all the services started.
    logprint "unblock ${OPENFIRE_PORT} port in case previous rule not deleted."
    ${IPTABLES} -D INPUT -i eth+ -p tcp -m  tcp --dport ${OPENFIRE_PORT} -m state --state NEW -j REJECT
    logprint "block ${OPENFIRE_PORT} port"       
    ${IPTABLES} -A INPUT -i eth+ -p tcp -m  tcp --dport ${OPENFIRE_PORT} -m state --state NEW -j REJECT    
    
    # Start Database Server
    ${IPVSXMLDBCTL} start

    # Start IPVS Component
    ${IPVSCOMPONENTCTL} start

    # There is no point confirming database really started. We start
    # Openfire anyway since we want openfire to be able to inform users of
    # any database problems

    # Start Openfire
    ${OPENFIRECTL} start

    # Start osgi
    ${OSGICTL} start

    #unblock 5222 port as we have started OSGI.
    logprint "unblock ${OPENFIRE_PORT} port"
    ${IPTABLES} -D INPUT -i eth+ -p tcp -m  tcp --dport ${OPENFIRE_PORT} -m state --state NEW -j REJECT
    
    # Register SC service
    register_sc_service

    return 0
}

stop_implementation() {


    # Dump logs - used only for debugging
    dump_logs

    # Unregister SC service
    unregister_sc_service
  
    # Start osgi
    ${OSGICTL} stop

    # Stop IPVS Component	
    ${IPVSCOMPONENTCTL} stop

    # Stop Openfire
    ${OPENFIRECTL} stop

    # Stop Sedna Server
    ${IPVSXMLDBCTL} stop

    return 0
}

reset_implementation() {

    # Send reset signal to  Openfire and Sednaserver
    logprint "Management Server System signaled for factory reset."
    ${OPENFIRECTL} reset
    ${IPVSXMLDBCTL} reset

}

status_implementation() {

    # Dump logs - used only for debugging
    if [ -f $DEBUG_FILE_PATH ] ; then
       dump_logs
       /bin/rm -rf ${DEBUG_FILE_PATH}
    fi 


    ret_status_reason=`$IPVSXMLDBCTL status`
    ret="$?"
    if [ "x$ret" != "x0" ]; then
	if [ "$ret_status_reason" != "STARTING" ]; then
            logprint "IPVS-XML-DB Server Returned Error '$ret_status_reason' "
            return 2
	fi
    fi

    ret_status_reason=`$OPENFIRECTL status`
    ret="$?"
    if [ "x$ret" != "x0" ]; then
	if [ "$ret_status_reason" != "STARTING" ]; then
            logprint "Openfire Server Returned Error '$ret_status_reason' "
            return 3
	fi
    fi

    ret_status_reason=`$IPVSCOMPONENTCTL status`
    ret="$?"
    if [ "x$ret" != "x0" ]; then
	if [ "$ret_status_reason" != "STARTING" ]; then
            logprint "IpvsComponent Server Returned Error '$ret_status_reason' "
            return 4
	fi
    fi

    ret_status_reason=`$OSGICTL status`
    ret="$?"
    if [ "x$ret" != "x0" ]; then
	if [ "$ret_status_reason" != "STARTING" ]; then
	    echo "ret_status_reason=" $ret_status_reason
            #logprint "osgi Server Returned Error '$ret_status_reason' "
            return 5
	fi
    fi

    return 0
}

handle_command $1
