#!/bin/sh

# Disable current network configuration
/sbin/ifconfig eth0 down
/sbin/ifconfig flan0 down
/V2O/bin/loadnet -rD

# Copy Default files to config directory 
/bin/cp /etc/luttable.txt /V2O/config
/bin/cp /etc/netconf.txt /V2O/config
/bin/cp /etc/routeconf.txt /V2O/config

# Remove DNS config file if exist
if [ -f /etc/resolv.conf ]
then
    /bin/rm -f /etc/resolv.conf
fi

# Force reconfigure defaults
/V2O/bin/loadlut
/V2O/bin/loadnet
/V2O/bin/loadnet -r

