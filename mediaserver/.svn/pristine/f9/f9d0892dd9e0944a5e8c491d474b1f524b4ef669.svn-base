#!/bin/sh

# Discovers SC on the network using avahi and/or DNS

# Source common library
. /etc/scripts/common.rc


logprint "Starting SC Service Discovery Daemon"

reannounce_file="/var/status/reannounce.sc"

_SEND_NOTIFICATION_EXEC="send_notification"
_SEND_NOTIFICATION_SRC="ServiceDiscovery"
_TYPE_DISCOVERY_COMPLETE="SCDiscoveryComplete"
_TYPE_DISCOVERY_ERROR="SCDiscoveryError"

SC_IP_ADDRESS=
LAST_SC_IP_ADDRESS=
UNKNOWN_SC_IP_ADDRESS="0.0.0.0"
SC_DOMAIN_NAME_PREFIX="barcosc"
SERIAL_NUMBER=`cat /etc/config/device_serial_number`

############## send_notification ##############
send_notif() {
    #send notification to device interface  
    local event_type="${1}"
    local foundsc="${2}"
    local ipaddress="${3}"
    local error="${4}"

    local nohup_log="/tmp/deviceinterface_send_notif.log"
    /bin/rm -f ${nohup_log}
    
   
    ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "foundsc" "${foundsc}" "ipaddress" "${ipaddress}" "serialnumber" "${SERIAL_NUMBER}" "error" "${error}"    > ${nohup_log} 2>&1 
    if [ "x$?" != "x0" ]; then
        result_error=`cat ${nohup_log} 2>&1`
        logprint "Failed to send_notification : ${event_type} error=${error} "   
    fi
    return 0  
}

discover_sc_using_avahi() {
    # Use avahi-browse to look for _BarcoSC._tcp service
    # Grab the IP address of the first one we find
    local sc_string=`avahi-browse -rtp _BarcoSC._tcp | grep -E "^=;" | head -1`
    if [ x${sc_string} != x ] ; then
        SC_IP_ADDRESS=`echo ${sc_string} | cut -d ';' -f 8`
    else
    	SC_IP_ADDRESS=""
    fi
}

discover_sc_using_dns() {
    # Do we have an SC yet?
    if [ "x${SC_IP_ADDRESS}" != x ]; then
   		return 
    fi
    
    local sc_string=`nslookup ${SC_DOMAIN_NAME_PREFIX} | tail -n +3 | grep "Address 1:" | cut -d " " -f 3`
    
    if [ "x${sc_string}" != x ] ; then
    	logprint "DNS resolved SC ${sc_string}"
       	SC_IP_ADDRESS=${sc_string}
   	else
   	   	SC_IP_ADDRESS=""
    fi
}


announce_sc() {
    # Did we get an SC?
    if [ "x${SC_IP_ADDRESS}" = x ]; then
    	if [ "x${LAST_SC_IP_ADDRESS}" = x${UNKNOWN_SC_IP_ADDRESS} ]; then
    		return
    	fi
    	logprint "SC not found, return unknown SC ${UNKNOWN_SC_IP_ADDRESS}"
    	LAST_SC_IP_ADDRESS=${UNKNOWN_SC_IP_ADDRESS}
    	send_notif "${_TYPE_DISCOVERY_ERROR}"  "false" "${UNKNOWN_SC_IP_ADDRESS}" "Cannot find SC!"
        return
    fi

	# Do we need to reannounce the SC?
	if [ -f $reannounce_file ]; then
		logprint "Re-Announcing SC ${SC_IP_ADDRESS}"
		send_notif "${_TYPE_DISCOVERY_COMPLETE}"  "true" "${SC_IP_ADDRESS}" ""
		return
	fi
	
	# Did the SC change since we last scanned?
    if [ "x${SC_IP_ADDRESS}" = x${LAST_SC_IP_ADDRESS} ]; then
        return
    fi

    LAST_SC_IP_ADDRESS=${SC_IP_ADDRESS}
    
    # This is where we annouce the discovered sc to the device interface
    logprint "Announcing SC ${SC_IP_ADDRESS}"
    if [ ! -f /etc/config/arm_system ] ; then
        send_notif "${_TYPE_DISCOVERY_COMPLETE}"  "true" "${SC_IP_ADDRESS}" ""
    else
        echo ${SC_IP_ADDRESS} > /etc/config/sc
        touch /etc/config/LoginRequest.xml

    fi
}


while (true)
do

    discover_sc_using_avahi

   	discover_sc_using_dns

    announce_sc

    sleep 5

done
