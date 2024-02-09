#!/bin/sh

# Source common library
. /etc/scripts/common.rc

# If local.txt file exists, we upgraded from a DMS120, DMS1000, etc...
# We convert the name-value pairs in local.txt into the 
# equivalent V2D config format.

if [ -r /tmp/local.txt ] ; then

    logprint "Converting DMS configuration file (local.txt) to V2D format..."

    # Fix dos end-of-lines
    /bin/sed -i -e 's/
$//' /tmp/local.txt

    # Primary IP address
    ipaddress=`/bin/grep '^ipaddress=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(ETHERIP *\).*/\1${ipaddress}/" /V2O/config/netconf.txt
    mask=`/bin/grep '^mask=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(ETHERMASK *\).*/\1${mask}/" /V2O/config/netconf.txt
    servername=`/bin/grep '^servername=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(HOSTNAME *\).*/\1${servername}/" /V2O/config/netconf.txt
    /bin/sed -i -e "s/\(TELNET *\).*/\1No/" /V2O/config/netconf.txt

    # Default route
    gw=`/bin/grep '^gw=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/rm /V2O/config/routeconf.txt
    echo "ROUTE_ID.1                1" > /V2O/config/routeconf.txt
    echo "DESTINATION.1             0.0.0.0" >> /V2O/config/routeconf.txt
    echo "NETMASK.1                 0.0.0.0" >> /V2O/config/routeconf.txt
    echo "GATEWAY.1                 ${gw}" >> /V2O/config/routeconf.txt

    # Secondary IP address
    /bin/sed -i -e "s/\(ENABLE *\).*/\1Yes/" /V2O/config/secondary_netconf.txt
    gwipaddress=`/bin/grep '^gwipaddress=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(IPADDRESS *\).*/\1${gwipaddress}/" /V2O/config/secondary_netconf.txt
    gwnetmask=`/bin/grep '^gwnetmask=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(NETMASK *\).*/\1${gwnetmask}/" /V2O/config/secondary_netconf.txt

    # DHCP Server
    if /bin/grep '^usedhcpd=1' /tmp/local.txt > /dev/null ; then
        /bin/sed -i -e "s/\(ENABLE *\).*/\1Yes/" /V2O/config/dhcpserverconf.txt
    fi
    dhcpdstartip=`/bin/grep '^dhcpdstartip=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(START_IP *\).*/\1${dhcpdstartip}/" /V2O/config/dhcpserverconf.txt
    dhcpdendip=`/bin/grep '^dhcpdendip=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(END_IP *\).*/\1${dhcpdendip}/" /V2O/config/dhcpserverconf.txt
    dhcpddns=`/bin/grep '^dhcpddns=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(DNS_SERVER *\).*/\1${dhcpddns}/" /V2O/config/dhcpserverconf.txt

    # NTP server
    if /bin/grep '^usentpd=1' /tmp/local.txt > /dev/null ; then
        /bin/sed -i -e "s/\(ENABLE *\).*/\1Yes/" /V2O/config/ntpconf.txt
    fi
    ntpserverip=`/bin/grep '^ntpserverip=' /tmp/local.txt | cut -d '=' -f 2`
    /bin/sed -i -e "s/\(IPADDRESS *\).*/\1${ntpserverip}/" /V2O/config/ntpconf.txt

    # Set the HW/SW profile
    /bin/touch /etc/config/sw_profile_dms.txt

else
    # This was an upgrade from a V2DTx/RX and all legacy config is 
    # already where we need it
    logprint "V2D configuration files detected..."

    # Set the HW/SW profile
    /V2O/bin/get_version | grep "V2D Receiver"> /dev/null
    if [ x$? != "x0" ]; then
	/bin/touch /etc/config/hw_profile_v2dtx.txt
    else
        /bin/touch /etc/config/hw_profile_v2drx.txt	
    fi
fi

#
# Convert V2D config files to Device.xml
#
logprint "Creating Device.xml..."

# First call the dumpConfig command that will write all the 
# config to /tmp/configfile
REMOTE_CONFIG=/etc/config/remote_config.xml
CONFIG_FILE=/tmp/configfile
UPGRADEDCONFIG_FILE=/etc/config/Device.xml
/bin/cp ${REMOTE_CONFIG} ${REMOTE_CONFIG}.save
sed -i -e 's/<Use>.*<\/Use>/<Use>File<\/Use>/' ${REMOTE_CONFIG}
sed -i -e 's/<Mode>.*<\/Mode>/<Mode>append<\/Mode>/' ${REMOTE_CONFIG}
if [ -e ${CONFIG_FILE} ]; then
    /bin/rm ${CONFIG_FILE}
fi
/V2O/bin/dump_config
/bin/mv ${REMOTE_CONFIG}.save ${REMOTE_CONFIG}

# Make the /tmp/configfile look like one xml document rooted at <CLIConfigs>
/bin/echo "<CLIConfigs>" > ${CONFIG_FILE}.save
/bin/grep -v '<?xml version="1.0"' ${CONFIG_FILE} >> ${CONFIG_FILE}.save
/bin/echo "</CLIConfigs>" >> ${CONFIG_FILE}.save
/bin/mv ${CONFIG_FILE}.save ${CONFIG_FILE}

# Now call the java config converter code to create the Device.xml file
AGENT=`get_agent_app_name`


java -cp /home/onair/modules/mediaroom/public/mediaroom.jar com.ipvs.device/${AGENT} UpgradeConfig ${CONFIG_FILE} ${UPGRADEDCONFIG_FILE} > /var/log/deviceapp/create_device_xml.log 2>&1


# Now that we have created our first Device.xml file, save it to flash
/bin/cfmount
/bin/cp ${UPGRADEDCONFIG_FILE} /mnt/cf/Device.xml
/bin/cfunmount

logprint "Created Device.xml"
