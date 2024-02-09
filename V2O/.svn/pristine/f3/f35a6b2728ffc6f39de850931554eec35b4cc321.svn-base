#!/bin/sh

# Disable current network configuration
#/sbin/ifconfig eth0 down
#/V2O/bin/loadnet -rD

# Copy Default files to config directory 
/bin/cp /etc/luttable.txt /V2O/config
/bin/cp /etc/netconf.txt /V2O/config
/bin/cp /etc/routeconf.txt /V2O/config
/bin/cp /etc/clientconf.txt /V2O/config
/bin/cp /etc/serverconf.txt /V2O/config
/bin/cp /etc/vidconf.txt /V2O/config
/bin/cp /etc/serverlistconf.txt /V2O/config
/bin/cp /etc/authlistconf.txt /V2O/config
/bin/cp /etc/ssh/sshd_config /V2O/sshd_config

# Force reconfigure defaults
#/V2O/bin/loadlut
#/V2O/bin/loadnet
#/V2O/bin/loadnet -r
#/V2O/bin/loadnet -S
