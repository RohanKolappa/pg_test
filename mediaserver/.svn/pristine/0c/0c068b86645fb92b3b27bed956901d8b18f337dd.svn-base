#!/bin/sh
#These have to be defined
#------------------------

TESTPREFIX=""
SCRIPT_PREFIX="/etc/scripts/watchdog"

#TESTPREFIX="/tmp/miyer"
#SCRIPT_PREFIX="/home/miyer/work_17146/mediaserver/system"


SERVER_NAME="DNSServer"
SERVER_CTL_FILE_PREFIX="dns"
SERVER_STATUS_FILE_PREFIX="dnsserver"
THIS_SCRIPT_PATH="${SCRIPT_PREFIX}/dnsctl.sh"

DEVICEAPP_HA_FILE="/usr/local/exist/deviceapp.ha.conf"

#DNS Constants
DNS_CONF_TEPLATE_FILE=/home/onair/templates/ms.named.conf.template
DNS_ZONE_TEPLATE_FILE=/home/onair/templates/ms.zone.template
DNS_IPVS_SUB_DOMIAN_KEY="_IPVS_SUB_DOMAIN_"
DNS_CORPOATE_DOMAIN_KEY="_CORPOATE_DOMAIN_"
DNS_IP_ADDRESS_KEY="_IP_ADDRESS_"
DNS_IPVS_SUB_DOMAIN_VALUE="ipvs."
DNS_CORPOATE_DOMAIN_VALUE="example.com"
DNS_BIND_HOME="/usr/local/bind"
DNS_CONF_DIR_PATH="${DNS_BIND_HOME}/etc"
DNS_CONF_FILE="${DNS_CONF_DIR_PATH}/named.conf"
DNS_ZONE_FILE="${DNS_CONF_DIR_PATH}/${DNS_IPVS_SUB_DOMAIN_VALUE}${DNS_CORPOATE_DOMAIN_VALUE}.zone"
DNS_CONF_ACTIVE_FILE="${DNS_CONF_FILE}.active"
DNS_ZONE_ACTIVE_FILE="${DNS_ZONE_FILE}.active"
DNS_CONF_REPLICATE_FILE="${DNS_CONF_FILE}.replicate"
DNS_ZONE_REPLICATE_FILE="${DNS_ZONE_FILE}.replicate"


LINUXHACONFIG_FILE="/V2O/config/linuxhaconf.txt"


source "${SCRIPT_PREFIX}/cmdctlbase.sh"

start_command() {    

    if [ -f $LINUXHACONFIG_FILE ] ; then
        peerIP=`/bin/grep PEERIP ${LINUXHACONFIG_FILE}  | tr -s " " | cut -d" " -f2`
    fi
    logprint "${peerIP} peerIP ..."
    #if there is replication then replace the local ip with peer ip.
    if [ -f $DEVICEAPP_HA_FILE ] ; then
      	replicate=`/bin/grep REPLICATE ${DEVICEAPP_HA_FILE}  | cut -d"=" -f2`
        logprint "${replicate} replicate ..."
      	/bin/rm -f $DBCONF_FILE
      	/bin/rm -f $DNS_ZONE_FILE
      	if [ x$replicate = "xtrue" ] ; then
            /bin/ln -s $DNS_CONF_REPLICATE_FILE $DNS_CONF_FILE 
            /bin/ln -s $DNS_ZONE_REPLICATE_FILE $DNS_ZONE_FILE 
      	else
            /bin/ln -s $DNS_CONF_ACTIVE_FILE $DNS_CONF_FILE 
            /bin/ln -s $DNS_ZONE_ACTIVE_FILE $DNS_ZONE_FILE 
        fi
    fi
    
    #todo: create a new user called 'named' 
    CMD="${DNS_BIND_HOME}/sbin/named -c ${DNS_BIND_HOME}/etc/named.conf -u root "
    eval $CMD > /dev/null 2>&1
    PID=`cat /var/run/named.pid`
    #echo "start_command PID=${PID}"
    return 0
}


handle_command $1
