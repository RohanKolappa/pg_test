#!/bin/sh


# Source common library
. /etc/scripts/common.rc

logprint "Retrieving configuration..."

# Legacy progress marker
# APPBOOT OK
/usr/bin/logger -p local6.notice "${APPBOOTOK} : APPBOOT OK"
echo ${APPBOOTOK} > ${LEDFILE}

#
# Mount compact flash and 
# copy any files we may need into /tmp
#
/bin/cfmount

if [ -r /mnt/cf/configs.tgz ] ; then
   /bin/cp /mnt/cf/configs.tgz /tmp/
   logprint "Found configs.tgz in compact flash"
fi

if [ -r /mnt/cf/local.txt ] ; then
    if [ ! -r /mnt/cf/bzImage ] ; then
        /bin/cp /mnt/cf/local.txt /tmp/
        logprint "Found local.txt in compact flash"
    fi
fi

if [ -r /mnt/cf/Device.xml ] ; then
    /bin/cp /mnt/cf/Device.xml /tmp/
    logprint "Found Device.xml in compact flash"
fi

if [ -r /mnt/cf/factory ] ; then
    /bin/touch /etc/config/factory
    logprint "Detected factory defaults"
fi

if [ -d /mnt/cf/cache ] ; then
    mkdir -p /tmp/cache
    /bin/cp /mnt/cf/cache/* /tmp/cache/
    logprint "Found cache files in compact flash"
fi

# TODO: Create Serial Number if not found on compact flash

# Unmount compact flash 
/bin/cfunmount


# Though Device.xml is the only config file we use, we untar
# config.tgz any way
if [ -r /tmp/configs.tgz ] ; then
    /bin/tar -C /tmp -xzvf /tmp/configs.tgz
    /bin/cp /tmp/configs/*.txt /V2O/config
	mkdir /V2O/scripts
    /bin/cp /tmp/configs/*.script /V2O/scripts
    /bin/cp /tmp/configs/sshd_config /V2O/config
    /bin/cp /tmp/configs/*_key /usr/local/etc
    /bin/cp /tmp/configs/*_key.pub /usr/local/etc
    rm -rf /tmp/configs
fi

# Copy cache files found in flash to final location
if [ -d /tmp/cache ] ; then
    /bin/cp  /tmp/cache/* /V2O/config/
    rm -rf /tmp/cache
fi


#
# If Device.xml exists then we are done.
#
if [ -r /tmp/Device.xml ] ; then

    cp /tmp/Device.xml /etc/config

else
    # Device.xml did not exist. We probably just got
    # upgraded and need to create Device.xml from legacy config files.
    ${SCRIPTS}/create_device_xml.sh
fi

if [ -f  ${MIGRATIONSCRIPTS}/migrate_device_xml.sh ]; then
    ${MIGRATIONSCRIPTS}/migrate_device_xml.sh
fi


# Determine what kind of device we are and create profile
# files for rest of the system to use
devname=`xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml`
case "$devname" in
	"ORB-500-Device")
		logprint "Detected ORB 500 device"
		/bin/touch /etc/config/sw_profile_orb.txt
		echo $devname > /etc/config/hw_profile_orb.txt
		;;
	"DMS-25-Device")
		logprint "Detected DMS 25 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-100-Device")
		logprint "Detected DMS 100 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-150-Device")
		logprint "Detected DMS 150 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-200-Device")
		logprint "Detected DMS 200 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1000-Device")
		logprint "Detected DMS 1000 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1200-Device")
		logprint "Detected DMS 1200 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1500-Device")
		logprint "Detected DMS 1500 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1550-Device")
		logprint "Detected DMS 1550 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1600-Device")
		logprint "Detected DMS 1600 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"DMS-1700-Device")
		logprint "Detected DMS 1700 device"
		/bin/touch /etc/config/sw_profile_dms.txt
		echo $devname > /etc/config/hw_profile_dms.txt
		;;
	"V2D-Tx-Device")
		logprint "Detected V2D Tx device"
		/bin/touch /etc/config/sw_profile_v2d.txt
		echo $devname > /etc/config/hw_profile_v2dtx.txt
		;;
	"V2D-Rx-Device")
		logprint "Detected V2D Rx device"
		/bin/touch /etc/config/sw_profile_v2d.txt
		echo $devname > /etc/config/hw_profile_v2drx.txt
		;;
	"V2D-XP200-Device")
		logprint "Detected V2D XP200 device"
		/bin/touch /etc/config/sw_profile_v2dxp200.txt
		echo $devname > /etc/config/hw_profile_v2dxp200.txt
		;;
	"V2D-XP200S-Device")
		logprint "Detected V2D XP200 device"
		/bin/touch /etc/config/sw_profile_v2dxp200.txt
		echo $devname > /etc/config/hw_profile_v2dxp200.txt
		;;
	"V2D-XP220-Device")
		logprint "Detected V2D XP200 device"
		/bin/touch /etc/config/sw_profile_v2dxp200.txt
		echo $devname > /etc/config/hw_profile_v2dxp200.txt
		;;
	"V2D-XP220S-Device")
		logprint "Detected V2D XP200 device"
		/bin/touch /etc/config/sw_profile_v2dxp200.txt
		echo $devname > /etc/config/hw_profile_v2dxp200.txt
		;;
	"V2D-XP100-Device")
		logprint "Detected V2D XP100 device"
		/bin/touch /etc/config/sw_profile_v2dxp200.txt
		echo $devname > /etc/config/hw_profile_v2dxp200.txt
		;;
	"V2D-XPi-Device")
		logprint "Detected V2D XPi device"
		/bin/touch /etc/config/sw_profile_xpi.txt
		echo $devname > /etc/config/hw_profile_xpi.txt
		;;
	"MS-25-Device")
		logprint "Detected MS 25 device"
		/bin/touch /etc/config/sw_profile_ms.txt
		echo $devname > /etc/config/hw_profile_ms.txt
		;;
	"MS-150-Device")
		logprint "Detected MS 150 device"
		/bin/touch /etc/config/sw_profile_ms.txt
		echo $devname > /etc/config/hw_profile_ms.txt
		;;
	"MS-Device")
		logprint "Detected MS device"
		/bin/touch /etc/config/sw_profile_ms.txt
		echo $devname > /etc/config/hw_profile_ms.txt
		;;
	"Generic-Device")
		logprint "Detected Generic device"
		/bin/touch /etc/config/sw_profile_generic.txt
		/bin/touch /etc/config/hw_profile_generic.txt
		;;
	*)
		logprint "Unable to determine Device Type, assuming MS 100"
		/bin/touch /etc/config/sw_profile_dms.txt
		/bin/touch /etc/config/hw_profile_ms.txt
		;;
esac
# Since xsltproc does not work yet, we force an XPI device
if [ -f /etc/config/arm_system ] ; then
    /bin/rm -f /etc/config/sw_profile*.txt
    /bin/rm -f /etc/config/hw_profile*.txt
    /bin/touch /etc/config/sw_profile_xpi.txt
    echo "V2D-XPi-Device" > /etc/config/hw_profile_xpi.txt
fi

# APPCONFIG OK
/usr/bin/logger -p local6.notice "${APPCONFIGOK} : APPCONFIG OK"
echo ${APPCONFIGOK} > ${LEDFILE}





