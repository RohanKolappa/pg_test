#!/bin/sh

SCRIPT_PREFIX="/etc/scripts/watchdog"

SERVER_NAME="Linux-HA"
SERVER_CTL_FILE_PREFIX="linuxha"
SERVER_STATUS_FILE_PREFIX="linuxha"

THIS_SCRIPT_PATH="${SCRIPT_PREFIX}/watch_linuxha.sh"
HEARTBEAT="/etc/init.d/heartbeat"

NEWHACONF_FILE="/etc/ha.d/ha.cf.new"
HACONF_FILE="/etc/ha.d/ha.cf"
NEWHARES_FILE="/etc/ha.d/haresources.new"
HARES_FILE="/etc/ha.d/haresources"
NEWAUTHKEYS_FILE="/etc/ha.d/authkeys.new"
AUTHKEYS_FILE="/etc/ha.d/authkeys"

DBCONF_TEMPLATE_FILE="/home/onair/templates/exist.conf"
DBCONF_FILE="/usr/local/exist/conf.xml"
DBCONF_ACTIVE_FILE="/usr/local/exist/active.conf.xml"
DBCONF_REPLICATE_FILE="/usr/local/exist/replicate.conf.xml"

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

DEVICEAPP_HA_FILE="/usr/local/exist/deviceapp.ha.conf"
DEVICEAPP_HA_ACTIVE_FILE="/usr/local/exist/deviceapp.ha.active.conf"
DEVICEAPP_HA_REPLICATE_FILE="/usr/local/exist/deviceapp.ha.replicate.conf"
DEVICEAPP_LOGIN_FILE="/etc/config/LoginRequest.xml"

#this file is watchable by watchdog. If this file present watch dog will start watching the management service.
#if device is in secondary this file going to be not present to stop unnecessary starts of management server on seconday
WATCH_MS="/var/run/watchms"

MSCTLEX_FILE="/usr/local/exist/msctlex.conf"

IPVSXMLDBCTL="/etc/scripts/ipvsxmldbctl.sh"

CLUSTERCTL="/etc/scripts/watchdog/clusterctl.sh"
DNSCTL="/etc/scripts/watchdog/dnsctl.sh"
MSERVERCTL="/etc/scripts/watchdog/watch_managementserver.sh"
DEVICEAPPCTL="/etc/scripts/watchdog/watch_deviceapp.sh"

XMPPSERVERCONFIG_FILE="/V2O/config/xmppserverconf.txt"
LINUXHACONFIG_FILE="/V2O/config/linuxhaconf.txt"

DBDIR_PATH="/data/existdbserver/"

#status  and flags for 
_FOLDER_IPVS_DB_BACKUP="/data/sedna_dbsnapshot"
_FOLDER_IPVS_DB_BACKUP_STATUS="${_FOLDER_IPVS_DB_BACKUP}/status"
_FILE_FLAG_TAKEOVER_0N_REBOOT="${_FOLDER_IPVS_DB_BACKUP_STATUS}/ha_takeover_on_reboot" 

source "${SCRIPT_PREFIX}/ctlbase.sh"

check_mserverctl_stop() {

    logprint "${SERVER_NAME} check_mserverctl_off ..."
    $MSERVERCTL off
    $MSERVERCTL waitUntilStopped 
}

check_mserverctl_restart() {

    logprint "${SERVER_NAME} check_mserverctl_on ..."

    # Check if xmpp disabled
    if [ -f $XMPPSERVERCONFIG_FILE ] ; then
        xmpp_enabled=`/bin/grep '^ENABLE' ${XMPPSERVERCONFIG_FILE} | grep -i yes`
        if [ x$? != "x0" ] ; then
            return 0;
        fi
    fi

    logprint "${SERVER_NAME}:check_mserverctl_on setting onafterstop"
    $MSERVERCTL onafterstop
}

generate_ha_cfg_files() {
    logprint "${SERVER_NAME} generate_ha_cfg_files ..."
    # dns.cfg
    # cluster.cfg

    peerIP="127.0.0.1"
    if [ -f $LINUXHACONFIG_FILE ] ; then
        peerIP=`/bin/grep PEERIP ${LINUXHACONFIG_FILE}  | tr -s " " | cut -d" " -f2`
    fi


    /bin/cp -f $DBCONF_TEMPLATE_FILE $DBCONF_ACTIVE_FILE
    sed -i -e 's/ip_mcast=true/ip_mcast=false;bind_port=5555/' -e 's/database=\"native\"/database=\"native_cluster\"/' -e "s/PING(/PING(gossip_host=127.0.0.1;gossip_port=5555;gossip_refresh=15000;/" ${DBCONF_ACTIVE_FILE}
    /bin/cp -f $DBCONF_TEMPLATE_FILE $DBCONF_REPLICATE_FILE
    sed -i -e 's/ip_mcast=true/ip_mcast=false;bind_port=5555/' -e 's/database=\"native\"/database=\"native_cluster\"/' -e "s/PING(/PING(gossip_host=${peerIP};gossip_port=5555;gossip_refresh=15000;/" ${DBCONF_REPLICATE_FILE}

    secondary="false"
    if [ -f $LINUXHACONFIG_FILE ] ; then
        primary=`/bin/grep ENABLE_AS_PRIMARY ${LINUXHACONFIG_FILE}  | tr -s " " | cut -d" " -f2`
        logprint "${SERVER_NAME} set db,deviceapp conf=replicate ..."
        # initial state of the files
        if [ x$primary = "xNo" ]; then
            secondary="true"
        fi
    fi

    if [ ${secondary} -eq "false" ] ; then
        touch ${WATCH_MS}	
        logprint "Primary server. start management server services ${WATCH_MS}"
    fi
  
    localIP="127.0.0.1"
    echo -e "REPLICATE=false\nSECONDARY=${secondary}\nDBDIR=${DBDIR_PATH}" > $DEVICEAPP_HA_ACTIVE_FILE
    echo -e "REPLICATE=true\nSECONDARY=${secondary}\nDBDIR=${DBDIR_PATH}" > $DEVICEAPP_HA_REPLICATE_FILE
    
    #Create DNS Templates
    /bin/cp -f $DNS_CONF_TEPLATE_FILE $DNS_CONF_ACTIVE_FILE
    /bin/cp -f $DNS_ZONE_TEPLATE_FILE $DNS_ZONE_ACTIVE_FILE
    /bin/cp -f $DNS_CONF_TEPLATE_FILE $DNS_CONF_REPLICATE_FILE
    /bin/cp -f $DNS_ZONE_TEPLATE_FILE $DNS_ZONE_REPLICATE_FILE
    #update dns template
    local DNS_IP_ADDRESS_VALUE=`_get_ipaddress`
    _update_dns_template $DNS_IP_ADDRESS_VALUE $DNS_CONF_ACTIVE_FILE 
    _update_dns_template $DNS_IP_ADDRESS_VALUE $DNS_ZONE_ACTIVE_FILE 
    local DNS_IP_ADDRESS_VALUE=$peerIP
    _update_dns_template $DNS_IP_ADDRESS_VALUE $DNS_CONF_REPLICATE_FILE 
    _update_dns_template $DNS_IP_ADDRESS_VALUE $DNS_ZONE_REPLICATE_FILE 
}

_update_dns_template() {
    local DNS_IP_ADDRESS_VALUE=$1
    local file_name=$2
    sed -i -e "s/${DNS_IPVS_SUB_DOMIAN_KEY}/${DNS_IPVS_SUB_DOMAIN_VALUE}/g" $file_name
    sed -i -e "s/${DNS_CORPOATE_DOMAIN_KEY}/${DNS_CORPOATE_DOMAIN_VALUE}/g" $file_name
    sed -i -e "s/${DNS_IP_ADDRESS_KEY}/${DNS_IP_ADDRESS_VALUE}/g" $file_name
}

remove_ha_cfg_files() {

    logprint "${SERVER_NAME} generate_standalone_cfg_files ..."
    /bin/rm -f $DBCONF_ACTIVE_FILE
    /bin/rm -f $DBCONF_REPLICATE_FILE

    /bin/rm -f $DEVICEAPP_HA_ACTIVE_FILE
    /bin/rm -f $DEVICEAPP_HA_REPLICATE_FILE
    
    /bin/rm -f $DNS_CONF_FILE
    /bin/rm -f $DNS_ZONE_FILE

    /bin/rm -f $DNS_CONF_ACTIVE_FILE
    /bin/rm -f $DNS_CONF_REPLICATE_FILE
    /bin/rm -f $DNS_ZONE_ACTIVE_FILE
    /bin/rm -f $DNS_ZONE_REPLICATE_FILE

}

start_implementation() {

	/bin/rm -f $WATCH_MS
	
    logprint "${SERVER_NAME} start_implementation ..."

    # This code makes sure that the deviceapp is not started by start_application_services. 
    # Even if it is started the devappctl stop confirms that deviceapp is stopped. 
    # This is used to prevent the device logging in as a normal device. With the fix deviceapp starts
    # replication on the standby device.
    $DEVICEAPPCTL disabled

    # check if mserverctl needs to be stopped
    $MSERVERCTL stop 
 
    # Deviceapp should be stopped only after MS is stopped    	
    devappctl stop

    generate_ha_cfg_files

    # enable msctlex 
    touch $MSCTLEX_FILE

    # init the files to standby 
    /bin/rm -f $DBCONF_FILE
    /bin/ln -s $DBCONF_REPLICATE_FILE $DBCONF_FILE
    /bin/rm -f $DEVICEAPP_HA_FILE
    /bin/ln -s $DEVICEAPP_HA_REPLICATE_FILE $DEVICEAPP_HA_FILE
    
    # Make sure we remove the temporary file in order to start deviceapp. 
    # logprint "${SERVER_NAME} set db,deviceapp conf=replicate ..."                                                 
    $DEVICEAPPCTL enabled
    devappctl start
    
    # Copy the new config files written by the menu
    /bin/cp -f $NEWHACONF_FILE $HACONF_FILE
    /bin/cp -f $NEWHARES_FILE $HARES_FILE
    /bin/cp -f $NEWAUTHKEYS_FILE $AUTHKEYS_FILE
    /bin/chmod 400 $AUTHKEYS_FILE

    # Start linux-ha
    ${HEARTBEAT} start
    if [ -f $_FILE_FLAG_TAKEOVER_0N_REBOOT ]; then
        #call take_over
        logprint "${SERVER_NAME} is schedule to take over ... "
        ha_takeover 
        rm -rf $_FILE_FLAG_TAKEOVER_0N_REBOOT	
    fi
}


stop_implementation() {

    logprint "${SERVER_NAME} stop_implementation ..."

    # stop replication if running
    if [ -f $DEVICEAPP_HA_FILE ] ; then
        replicate=`/bin/grep REPLICATE ${DEVICEAPP_HA_FILE}  | cut -d"=" -f2`
        if [ x$replicate = "xtrue" ] ; then
            logprint "${SERVER_NAME} $IPVSXMLDBCTL stop ..."
            $IPVSXMLDBCTL stop
        else
            # if we are not replicating, mserverctl is on
            # check if mserverctl needs to be restarted
            check_mserverctl_restart
        fi
    fi
    /bin/rm -f $DEVICEAPP_HA_FILE


    remove_ha_cfg_files

    # put back the standalong config
    /bin/rm -f $DBCONF_FILE
    /bin/cp -f $DBCONF_TEMPLATE_FILE $DBCONF_FILE
    /bin/rm -f $DEVICEAPP_HA_FILE

    # mask msctlex 
    /bin/rm $MSCTLEX_FILE

    $CLUSTERCTL off 
    $DNSCTL off 
    $MSERVERCTL off 
    $CLUSTERCTL stop 
    $DNSCTL stop 
    $MSERVERCTL stop 

    # Stop linux-ha
    ${HEARTBEAT} stop
    
    # Trigger a devappctl restart. Required for updating resource status. 
    touch $DEVICEAPP_LOGIN_FILE
}


reset_implementation() {
	return 0
}
          
hup() {

    generate_ha_cfg_files

    ${HEARTBEAT} reload &
    ${CLUSTERCTL} stop &
    ${DNSCTL} stop &
    ${MSERVERCTL} stop &
}

#get_cl_status() {
#    /bin/rm -f /tmp/cl_status.log
#    cl_status hbstatus >> /tmp/cl_status.log
#    cl_status rscstatus >> /tmp/cl_status.log
#    /bin/rm -f /var/run/cl_status.lck
#}

_get_ipaddress() {
    #todo: will need more utility functions litk this... i.e. to get values from CLI-XML+XSL .. 
    #for now using dynamic xml/xsl !!!. should have been a common script file. confirm with Shanker/Rahul/Iyer
    local xml_get_network_ports_status=`get_network_ports_status`
    local  name_prefix=`uuidgen`
    local xml_file="/var/run/_get_ipaddress${name_prefix}.xml"
    local xsl_file="/var/run/_get_ipaddress${name_prefix}.xsl"
    echo $xml_get_network_ports_status > $xml_file
    echo "<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\"> "  > $xsl_file
    echo "<xsl:output method=\"text\" /> " >> $xsl_file
    echo "<xsl:template match=\"/\"><xsl:value-of select=\"//ParametersList/Parameters[Parameter[@name='Primary' and .='True']]/Parameter[@name='IPAddress']\"/></xsl:template> "  >> $xsl_file
    echo "</xsl:stylesheet>" >> $xsl_file
    local ipaddress=`xsltproc $xsl_file $xml_file `
    /bin/rm -rf /var/run/_get_ipaddress*
    echo $ipaddress
    return 0
}

status_implementation() {

	$IPVSXMLDBCTL cleanjournal	
    ret_status_reason=`$HEARTBEAT status`
    ret="$?"
    return $ret
}

status_resource() {
    if [ ! -f /etc/ha.d/haresources ]; then
        echo ""
        return 0
    fi
    local rcstatus=`cl_status rscstatus`
    local serverName=`uname -n`
    local checkPrimary=`grep $serverName /etc/ha.d/haresources | grep msctlex | wc -c`
    checkPrimary=`expr $checkPrimary + 0`
    #todo: may be rcstatus='all' does not mean its active nor standby ?
    if [ "$rcstatus" = "all" ]; then
        echo "Active"
    elif [ "$rcstatus" = "none"  ]; then
        echo "Standby"
    elif [ "$rcstatus" = "local" ]; then
        if [ $checkPrimary -gt 0 ]; then
            echo "Active"
        else
            echo "Standby"
        fi
    else
        echo ""
    fi
    return 0
}

scheduleTakeOverIfActive() {
    if [ ! -f /etc/ha.d/haresources ]; then
        echo ""
        return 0
    fi
    local ha_active=`status_resource`	
    if [ "${ha_active}" = "Active" ]; then
        touch $_FILE_FLAG_TAKEOVER_0N_REBOOT		
    fi
    return 0	
}

case "$1" in

    hup)
    #hup
    ;;

    hupinternal)
    hup
    ;;
    
    #get_cl_status)
    #get_cl_status
    #;;

    files)
    generate_ha_cfg_files
    ;;

    status_resource)
    status_resource
    ;;
		
    #schedule Take Over on reboot, only if its active
    scheduleTakeOverIfActive)
    scheduleTakeOverIfActive
    ;;


    *) handle_command $1

esac
