#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Starting System Services..."

# Start Front Panel LCD daemon
if [ -f /etc/config/hw_profile_dms.txt -o \
     -f /etc/config/hw_profile_ms.txt -o \
     -f /etc/config/hw_profile_generic.txt ] ; then
    if [ -f /sbin/frontpanel ] ; then 
        /sbin/frontpanel
    fi
	# Coping exist conf file from templates to /usr/local/exist
	if [ -f /home/onair/templates/exist.conf ] ; then
		/bin/cp -f /home/onair/templates/exist.conf /usr/local/exist/conf.xml
	fi
fi

# Coping exist conf file from templates to /usr/local/exist only ro MS & DMS devices
if [ -f /etc/config/hw_profile_dms.txt -o \
     -f /etc/config/hw_profile_ms.txt ] ; then
	if [ -f /home/onair/templates/exist.conf ] ; then
		/bin/cp -f /home/onair/templates/exist.conf /usr/local/exist/conf.xml
	fi
fi

# On XP-100 disable WOL (Wakeup On Lan) on the nic card
# This prevent network freeze on soft reboot
if [ -f /etc/config/hw_profile_v2dxp200.txt ] ; then
    grep "V2D-XP100-Device" /etc/config/hw_profile_v2dxp200.txt > /dev/null 2>&1
    if [ x$? = x0 ] ; then
       /sbin/ethtool -s eth0 wol d
    fi
fi


# Load fpga driver
if [ -f /etc/config/hw_profile_v2dxp200.txt ] ; then
    # Setup board PCI slots for XP220 device
    grep "V2D-XP220-Device" /etc/config/hw_profile_v2dxp200.txt > /dev/null 2>&1
    if [ x$? = x0 ] ; then
        echo "boardpcislots=12,14" > /tmp/boardpcislots
    fi
fi

if [ ! -f /etc/config/arm_system ] ; then
    /bin/drvload
fi

# Detect if this device is supposed to have a serial console
if [ -f /etc/config/hw_profile_v2dxp200.txt ] ; then
    # XP-100 device does not have a serial console
    grep "V2D-XP100-Device" /etc/config/hw_profile_v2dxp200.txt > /dev/null 2>&1
    if [ x$? = x0 ] ; then
        touch /etc/config/no_serial_console
    fi
fi

# Start Firewall
   /etc/scripts/iptables.sh

# Setup cron jobs 
if [ -f /home/onair/templates/crontab ] ; then
    /bin/cp /home/onair/templates/crontab /etc/crontab
    /usr/bin/crontab /etc/crontab
fi
