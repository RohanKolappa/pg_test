#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Starting Application Services..."


# Start web server
if [  -f /etc/config/sw_profile_dms.txt ] || [ -f /etc/config/sw_profile_ms.txt ] ; then
    logprint "Starting Web Server"
    OPENSSL=/bin/openssl
    cp /home/onair/templates/openssl.cnf /tmp/configfile
    cp /home/onair/templates/mime.types /home/onair/apache/conf/mime.types
    cp /home/onair/templates/crossdomain.xml /home/onair/htdocs/public/crossdomain.xml
    echo "CN=" `uname -n` >> /tmp/configfile
    ${OPENSSL} genrsa  -out /tmp/server.key 2048
    ${OPENSSL} req  -new -key  /tmp/server.key -out /tmp/server.csr -config /tmp/configfile
    ${OPENSSL} x509 -req -days 3650 -in /tmp/server.csr -signkey /tmp/server.key -out /tmp/server.crt
    cat /tmp/server.key /tmp/server.crt > /home/onair/keycert.pem
    rm /tmp/server.key /tmp/server.csr /tmp/server.crt /tmp/configfile
    /home/onair/apache/bin/httpd -DSSL -f /home/onair/httpd.conf > /dev/null 2>&1    
    forever start /home/onair/modules/mediajs/public/app.js -l /var/log/mediajs/node.log -a
fi
if [ -f /etc/config/arm_system ] ; then
    /bin/mkdir -p /var/www
    /usr/sbin/lighttpd -f /etc/lighttpd/lighttpd.conf
fi
if [ ! -f /etc/config/arm_system ] ; then
	logprint "Removing thumbnail cache"
    /bin/rm -rf /data/tncache/live/*
fi

# Start SSH server
if [ -f /etc/config/arm_system ] ; then
    /bin/touch /etc/issue.net
    /etc/init.d/S50dropbear start
fi

# Start NTP server
${SCRIPTS}/watchdog/watch_ntp.sh start


#
# Start Streaming Server
#

if [ ! -f /etc/config/hw_profile_generic.txt ] ; then
    ${SCRIPTS}/watchdog/watch_streamingserver.sh start
fi

if [ -f /etc/config/arm_system ] ; then
    if [ -e /home/onair/TV/bin/net2usb ] ; then
      ${SCRIPTS}/watchdog/watch_net2usb.sh start
    fi
    ${SCRIPTS}/watchdog/watch_network.sh start
fi

# PSCW -- Start script to watch I2C failure on XP100/XP200
if [ -f /etc/config/sw_profile_v2dxp200.txt ] ; then
    ${SCRIPTS}/watchdog/watch_i2c.sh start
fi

# PSCW -- Start HALD on XPi here for now
if [ -f /etc/config/arm_system ] ; then
    /V2O/bin/hald -b 0
fi

# PSCW -- Start ifplugd on XPi here for now
if [ -f /etc/config/arm_system ] ; then
    /usr/sbin/ifplugd -fI -u2 -d5
fi

# On V2D/XP devices kickoff StreamingServer in stand alone mode.
if [  -f /etc/config/arm_system ] ; then
    if [ -e /V2O/config/standaloneoperation.txt ]; then
      if grep -q No /V2O/config/standaloneoperation.txt; then
        if [ -f /etc/scripts/watchdog/watch_deviceapp.sh ]; then
          /etc/scripts/watchdog/watch_deviceapp.sh start
        fi
      fi
    else
      echo "STANDALONE        Yes" > /V2O/config/standaloneoperation.txt
    fi
fi

/V2O/bin/standalone

if [ ! -f /etc/config/arm_system ] ; then
    # Start Device Agent
    if [ ! -f /etc/config/hw_profile_generic.txt ] ; then
      if [ -f /etc/scripts/watchdog/watch_deviceapp.sh ]; then
        /etc/scripts/watchdog/watch_deviceapp.sh start
      fi
   fi


    #
    # Start the HDD monitor
    #
    if [ -f ${SCRIPTS}/watchdog/watch_hdd.sh ] ; then 
      ${SCRIPTS}/watchdog/watch_hdd.sh start
    fi
fi

# Start the SC discovery daemon
${SCRIPTS}/sc_discovery.sh > /dev/null 2>&1 &

# Create symbolic links for fake cli's created to support the In Box UI
if [ ! -f /etc/config/arm_system ] ; then
    if [ -d /usr/share/bweb/localui/scripts ] ; then
      /bin/ln -sf /usr/share/bweb/localui/scripts/describe_ldap_configuration.sh /V2O/bin/describe_ldap_configuration
      /bin/ln -sf /usr/share/bweb/localui/scripts/get_ldap_configuration.sh /V2O/bin/get_ldap_configuration
      /bin/ln -sf /usr/share/bweb/localui/scripts/set_ldap_configuration.sh /V2O/bin/set_ldap_configuration
      
	  /bin/ln -sf /usr/share/bweb/localui/scripts/describe_ms_server_options.sh /V2O/bin/describe_ms_server_options
      /bin/ln -sf /usr/share/bweb/localui/scripts/get_ms_server_options.sh /V2O/bin/get_ms_server_options
      /bin/ln -sf /usr/share/bweb/localui/scripts/set_ms_server_options.sh /V2O/bin/set_ms_server_options
	  
	  /bin/ln -sf /usr/share/bweb/localui/scripts/describe_dms_server_options.sh /V2O/bin/describe_dms_server_options
      /bin/ln -sf /usr/share/bweb/localui/scripts/get_dms_server_options.sh /V2O/bin/get_dms_server_options
      /bin/ln -sf /usr/share/bweb/localui/scripts/set_dms_server_options.sh /V2O/bin/set_dms_server_options

      /bin/ln -sf /usr/share/bweb/localui/scripts/describe_ha_configuration.sh /V2O/bin/describe_ha_configuration
      /bin/ln -sf /usr/share/bweb/localui/scripts/get_ha_configuration.sh /V2O/bin/get_ha_configuration
    fi
fi
