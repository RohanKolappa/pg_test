#!/bin/sh

# Turn on IP forwarding
/bin/echo 1 > /proc/sys/net/ipv4/ip_forward

# Start syslog daemon
/sbin/syslogd -C

# Start klogd daemon
/sbin/klogd

# Create character special file for accessing 21285 UART (Debug 3-pin console)
/bin/mknod /dev/ttyFB c 204 4

# Start telnet daemon
# Not yet!! Started dynamically
#/usr/sbin/telnetd
 

# Install vutils binaries (creates symbolic links)
/V2O/bin/vutils --install

# Find and Load configuration data from Flash
/V2O/bin/retrieveflash.sh

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

# Run postautorun script after loading all configuration
if [ -f /V2O/sbin/postautorun.sh ]
then
    /bin/echo "Running postautorun.sh"
    /V2O/sbin/postautorun.sh
fi
