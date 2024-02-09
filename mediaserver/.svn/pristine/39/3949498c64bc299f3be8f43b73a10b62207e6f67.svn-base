#!/bin/sh

# Source common library
. /etc/scripts/common.rc

#
# Note that hard disk is not yet mounted on /data when this script is called.
# Do not put anything here that assumes /data has persistent stuff
#

logprint "Setting up File System..."


# Change user and group ownership of all files to root
# (Else they will come out as belonging to 
# person who created the build)
if [ ! -f /etc/config/arm_system ] ; then
    for dir in /usr /bin /sbin /etc /home /dev /lib /mnt /var /tmp /init; do
       /bin/chown -R -h root:root $dir
    done
fi


# Set SUID bit on vutils (and hence make the menu commands 
# executable by non-root user)
/bin/chmod u+s /home/onair/V2O/bin/vutils

# Set SUID bit on busybox. This makes some comands (e.g. ping)
# usable by non root users
chmod u+s /bin/busybox

# Remove reboot and halt command links to busybox binary so 
# that they are not used accidently.
# These can always be used via busybox binary if needed
/bin/rm -f /sbin/reboot
/bin/rm -f /sbin/halt

# Setup kernel's version of userspace poweroff commmand
if [ -f /etc/scripts/kernel_poweroff.sh ] ; then
    /bin/ln -s /etc/scripts/kernel_poweroff.sh /sbin/kpwroff
    /sbin/sysctl -w kernel.poweroff_cmd="/sbin/kpwroff"
fi

# Make /tmp accessible to non root user too.
/bin/chmod a+rwx /tmp

# Create essential links.

# Mediaroom links
/bin/mkdir -p /home/onair/htdocs/public
/bin/ln -s /home/onair/modules/mediaroom/public /home/onair/htdocs/public/mediaroom

# Mediaroom links for HLS media
/bin/mkdir -p /home/onair/modules/mediaroom/public/HLS
/bin/chown admin:admin /home/onair/modules/mediaroom/public/HLS

###/bin/ln -s /home/onair/modules/player/public /home/onair/htdocs/public/player

if [ ! -f /etc/config/arm_system ] ; then
    # GUI download links
    clientsetupexe=`ls /home/onair/modules/clients/public/install/BarcoCollab*.exe`;
    if [ x${clientsetupexe} != "x" ]; then
        /bin/ln -s ${clientsetupexe} /home/onair/modules/clients/public/install/BarcoCollabClientSetup.exe 
    fi
    clientsetupexe=`ls /home/onair/modules/clients/public/install/Odyssey*.exe`;
    if [ x${clientsetupexe} != "x" ]; then
        /bin/ln -s ${clientsetupexe} /home/onair/modules/clients/public/install/OdysseyCinemaClientSetup.exe 
    fi
fi

# Link objectXml directory needed by GI
###mkdir -p /home/onair/modules/clients/public/GI/JSXAPPS/IPVS-Common/model
###/bin/ln -s /home/onair/objectXml /home/onair/modules/clients/public/GI/JSXAPPS/IPVS-Common/model/objectXml


# Link xsl and objectXml directories to /etc/scripts
/bin/ln -s /home/onair/xsl /etc/scripts/xsl
/bin/ln -s /home/onair/objectXml /etc/scripts/objectXml

# Openfire requirements
mkdir -p /var/log/openfire

# IpvsComponent App requirements
mkdir -p /var/log/ipvscomponent

# Device App requirements
mkdir -p /var/log/deviceapp

# mscomponent App requirements
mkdir -p /var/log/mscomponent

# bind named App requirements
mkdir -p /var/log/named


if [ ! -f /etc/config/arm_system ] ; then
    # Node App requirements
    mkdir -p /var/log/mediajs

    # Apache requirements
    /bin/mkdir -p /var/log/apachelogs
    /bin/ln -s /var/log/apachelogs /home/onair/apache/logs
fi

# Links for /V2O
/bin/ln -s /home/onair/V2O /V2O
/bin/ln -s /home/onair/lib/libv2okernel.so /lib
if [ -f /home/onair/lib/libthreadobjs.so ] ; then
    /bin/ln -s /home/onair/lib/libthreadobjs.so /lib
fi
/V2O/bin/vutils --install
if [ -f /V2O/bin/maint ] ; then
    /V2O/bin/maint --install
fi

# Link kernel modules directory
/bin/ln -s /home/onair/lib/modules/* /lib/modules

# Driver load/unload shortcuts
if [ -f /etc/scripts/drvload.sh ] ; then
    /bin/ln -s /etc/scripts/drvload.sh /bin/drvload
fi
if [ -f /etc/scripts/drvunload.sh ] ; then
    /bin/ln -s /etc/scripts/drvunload.sh /bin/drvunload
fi

# NAS requirements
/bin/mkdir -p /var/status
/bin/mkdir -p /var/lib/nfs
/bin/mkdir -p /var/lib/nfs/statd
/bin/mkdir -p /var/lib/rpcbind
/bin/chown rpc:rpc /var/lib/rpcbind

# HA requirements
HARUNDIR="/var/lib/heartbeat"
mkdir -p ${HARUNDIR}
mkdir -p ${HARUNDIR}/cores
mkdir -p ${HARUNDIR}/cores/root
mkdir -p ${HARUNDIR}/cores/nobody
mkdir -p ${HARUNDIR}/lrm

# This is needed for downgrade to 2.3 release using ./upgradems.sh
/bin/mkdir /tmp/floppy

# These were originally in dms_build/Makefile
/bin/ln -s /home/onair/lib/libv2d.so /lib
/bin/ln -s /home/onair/lib/libmediaindex.so /lib
/bin/ln -s /home/onair/lib/librfbproto.so /lib
/bin/ln -s /home/onair/lib/libv2o.so /lib
/bin/ln -s /home/onair/lib/libmediaconverter.so /lib
/bin/ln -s /home/onair/lib/libstrmsrv.so /lib


# Convenient shortcuts
if [ -f /etc/scripts/watchdog/watch_streamingserver.sh ] ; then
    /bin/ln -s /etc/scripts/watchdog/watch_streamingserver.sh /bin/strmsrvctl
fi

if [ -f /etc/scripts/watchdog/watch_deviceapp.sh ] ; then
    /bin/ln -s /etc/scripts/watchdog/watch_deviceapp.sh /bin/devappctl
fi

if [ -f /etc/scripts/watchdog/watch_managementserver.sh ] ; then
    /bin/ln -s /etc/scripts/watchdog/watch_managementserver.sh /bin/mserverctl
fi

if [ -f /etc/scripts/watchdog/watch_linuxha.sh ] ; then
    /bin/ln -s /etc/scripts/watchdog/watch_linuxha.sh /bin/hactl
fi

if [ -f /etc/scripts/ipvsxmldbctl.sh ] ; then
	#todo: remove after removing ref to sednactl from every other application	
    /bin/ln -s /etc/scripts/sednactl.sh /bin/sednactl
    /bin/ln -s /etc/scripts/ipvsxmldbctl.sh /bin/ipvsxmldbctl
fi

if [ -f /etc/scripts/osgictl.sh ] ; then
    /bin/ln -s /etc/scripts/osgictl.sh  /bin/osgictl
fi

if [ -f /etc/scripts/openfirectl.sh ] ; then
    /bin/ln -s /etc/scripts/openfirectl.sh /bin/openfirectl
fi

if [ -f /home/onair/ipvscomponent/bin/ipvscomponent.sh ] ; then
    /bin/ln -s /home/onair/ipvscomponent/bin/ipvscomponent.sh  /bin/ipvscomponentctl
fi

if [ -f /etc/scripts/watchdog/watch_nas.sh ] ; then
    /bin/ln -s /etc/scripts/watchdog/watch_nas.sh /bin/nasctl
fi

if [ -f /etc/scripts/installwizard.sh ] ; then
    /bin/ln -s /etc/scripts/installwizard.sh  /bin/installwizard
fi


# APPBOOTFILEOK OK
/usr/bin/logger -p local6.notice "${APPBOOTFILEOK} : APPBOOTFILE OK"
echo ${APPBOOTFILEOK} > ${LEDFILE}
