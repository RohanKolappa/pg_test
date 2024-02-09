#!/bin/sh

PATH=/bin:/sbin:/usr/bin:/usr/sbin:/V2O/bin:/V2O/sbin
TERM=vt100

export TERM PATH

# Turn on IP forwarding
#/bin/echo 1 > /proc/sys/net/ipv4/ip_forward

# Start syslog daemon
/sbin/syslogd -C

# Start klogd daemon
/sbin/klogd -c 1

# Install vutils binaries (creates symbolic links)
/V2O/bin/vutils --install

# Mount the flash device
/V2O/bin/mountflash.sh 

# Find and Load configuration data from Flash
/V2O/bin/retrieveflash.sh
/V2O/bin/retrieveflash_passwd.sh

# Run preautorun script before loading driver
if [ -f /V2O/sbin/preautorun.sh ]
then
    /bin/echo "Running preautorun.sh"
    /V2O/sbin/preautorun.sh
fi

# Load the fpga module into the kernel
/usr/bin/load.sh

# Run autorun script
if [ -f /V2O/sbin/autorun.sh ]
then
    /bin/echo "Running autorun.sh"
    /V2O/sbin/autorun.sh
fi

# Load initial configuration
/V2O/bin/loadlut

# Apply network configuration  (eth, flan IPs, telnet)
/V2O/bin/loadnet

# Apply route configuration
/V2O/bin/loadnet -r

# Apply SSH configuration
/bin/mkdir -p /var/run
/bin/mkdir -p /var/console
/V2O/bin/loadnet -S

# Apply video configuration
/V2O/bin/loadnet -v

# Add multicast route (Moved to loadnet.c)
#route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0

# Start vwhois daemon
/V2O/bin/vwhoisd

# Run postautorun script after loading all configuration
if [ -f /V2O/sbin/postautorun.sh ]
then
    /bin/echo "Running postautorun.sh"
    /V2O/sbin/postautorun.sh
fi
