#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Retrieving Application Images from compact flash..."

#
# Find the compact flash and load the application images from it
#
found_cf=0
retries=0
maxretries=5
index=0
cfgdevs="/dev/hde1 /dev/hda1 /dev/hdc1 /dev/hdd1 /dev/hdb1 /dev/mmcblk0p1 /dev/sda1 /dev/sdb1 /dev/sdc1"
while(true)
do
	for dev in ${cfgdevs}
	do
		/bin/mount ${dev} -t ${CF_FS_TYPE} /mnt/cf > /dev/null 2>&1

		if [ x$? = "x0" ]; then
	
			# Manifest file better be there
			if [ -r /mnt/cf/manifest.txt ] ; then
				logprint "Found IPVS Software in $dev"
				/bin/ln -s ${dev} /dev/CF
				found_cf=1
				if [ $CF_FS_TYPE == "ext3" ]; then 
				   touch /etc/config/grub_system
				fi
        
				# Untar all application tarballs listed as "type=app" in manifest file
				apps=`/bin/grep type=app /mnt/cf/manifest.txt | /bin/sed 's/.*file=\(.*\.tgz\).*/\1/'`
				for app in $apps ; do 
					logprint "Installing Image: $app ..."
					/bin/tar -xzmf /mnt/cf/$app
				done
            
				# Located compact flash so break out of loop
				break
			fi

			# Unmount this device and try the next one
			/bin/umount ${dev}
		fi

	done
	if [ "x$found_cf" = "x1" ]; then
		break;
	fi
	let "retries=$retries+1"
	if [ "x$retries" = "x1" ]; then
	   CF_FS_TYPE=vfat
	elif [ "x$retries" = "x2" ]; then
	   CF_FS_TYPE=ext3
	elif [ "x$retries" = "x$maxretries" ]; then
		logprint "Max retries exceeded: Could not find application images"
		break;
	else
		KERNEL_VERSION=`uname -r`
		echo "Workaround for Defect #28822"
		# /sbin/rmmod sdhci-pci
		# /sbin/insmod /lib/modules/${KERNEL_VERSION}/sdhci-pci.ko
	fi
	logprint "Waiting for Disk... $retries"
	sleep 5
done

#
# If compact flash not found, touch a file and return
#
if [ "x$found_cf" = "x0" ]; then
    /bin/touch /tmp/cf_not_found
    exit
fi


# If compact flash has our custom hook, run it
if [ -r /mnt/cf/custom_startup.sh ] ;
then
   logprint "Found Custom Startup Script"
   /bin/sh /mnt/cf/custom_startup.sh
fi

# Unmount CF
/bin/umount /mnt/cf


# APPIMAGE OK
/usr/bin/logger -p local6.notice "${APPIMAGEOK} : APPIMAGE OK"
echo ${APPIMAGEOK} > ${LEDFILE}

