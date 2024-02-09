#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Mounting Hard Drive..."

# Legacy progress marker
# ENCODEROK OK
/usr/bin/logger -p local6.notice "${ENCODEROK} : ENCODER OK"
echo ${ENCODEROK} > ${LEDFILE}

#
# Create software RAID.
# These commands can be run safely on all systems since they succeed
# only where applicable
#
#mknod /dev/md0 b 9 0
#/bin/mdadm -A /dev/md0 /dev/sda1 /dev/sdb1

# From fsck.sh:


makeDir()
{
	rootDir=$1;
	umask 0000
        /bin/mkdir -p ${rootDir}/DSS/data/file
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/db
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/blob
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/templates
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/videolibrary/db
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/thumbnails/1
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/thumbnails/2
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site-cache
	/bin/mkdir -p ${rootDir}/upload_tmp_dir
	/bin/mkdir -p ${rootDir}/upgrade_dir
	umask 0022
        if [ -f ${rootDir}/uptime.log ]; then
            /usr/bin/tail -3000 ${rootDir}/uptime.log > ${rootDir}/uptime.log.tmp
            /bin/mv ${rootDir}/uptime.log.tmp ${rootDir}/uptime.log
        fi
        /bin/echo -n "Device Up: " >> ${rootDir}/uptime.log
        /bin/date >> ${rootDir}/uptime.log
        /usr/bin/uptime >> ${rootDir}/uptime.log

	return
}

# This routine links /data to /home/onair/data which is on ramfs
# This creates more space on /data. 
link_to_ramfs()
{
    /bin/mkdir -p /home/onair/data
    /bin/rmdir /data
    /bin/ln -s /home/onair/data /
}

# Create software RAID.
# These commands can be run safely on all systems since they succeed
# only where applicable
#
if [ ! -f /etc/config/arm_system ] ; then
    mknod /dev/md0 b 9 0
    /bin/mdadm -A /dev/md0 /dev/sda1 /dev/sdb1
else 
    link_to_ramfs
    makeDir /data
    exit
fi

# Store the mount point in a special file
# Warning: For changing this file name see DRY.3
/usr/bin/xsltproc -o /etc/config/hd_mount_point.txt /etc/scripts/xsl/get_mount_points.xsl /etc/config/Device.xml

# Locate the hard drive partition(s)
datadevs="/dev/md0 /dev/hdc1 /dev/hda1 /dev/hdd1 /dev/sda1 /dev/sda2 /dev/sdb1 /dev/sdc1 /dev/sdd1 /dev/hda2 /dev/hda3"
/bin/mkdir -p /data2
found_hard_drive=0
DATAPARTITION=""
for dev in ${datadevs}
do
skipmount=`ls -al /dev/CF | sed -e '/CF/ s/.*\->*//g' | tr -d [:space:]`
    if [ $dev != "$skipmount" ] ; then 
       /bin/mount -t ext3 ${dev} /data2
       if [ $? -eq 0 ];
         then
         DATAPARTITION="${DATAPARTITION} ${dev}"
         /bin/umount /data2
         found_hard_drive=1
         logprint "Found Hard Drive on ${dev}"
      fi
	fi
done
echo ${DATAPARTITION}
do_reboot="false"
for i in `echo ${DATAPARTITION}`
do
	logprint "Checking Hard Drive (running e2fsck)... $i"
	/sbin/e2fsck -y ${i} >> /tmp/e2fsck.log
	tt=`grep Pass /tmp/e2fsck.log`
	if [ -n "$tt" ]; then
		do_reboot="true"
	fi
done
echo $do_reboot

# No hard drive found??
if [ "x$found_hard_drive" = "x0" ]
then
    logprint "No hard drive detected."
    link_to_ramfs
    makeDir /data
    exit
fi


if [ x"$do_reboot" = x"true" ]; then
    cat /tmp/e2fsck.log
    logprint "Hard Drive needed fixing. Rebooting device."
    /bin/sleep 10
    /bin/touch /tmp/reset
    /bin/sleep 10
    /bin/busybox reboot
    exit
fi

# In case /data is currently linked to anything, remove it
/bin/rm -rf /data > /dev/null 2>&1

mountnumber="0"
for i in `echo ${DATAPARTITION}`
do
# Mount the hard drive
	let "mountnumber=$mountnumber+1"
	mountpoint="/data"$mountnumber
	echo $mountpoint
	/bin/mkdir -p ${mountpoint}
	/bin/mount -t ext3 ${i} ${mountpoint}
	if [ $? != 0 ];
	then
		logprint "Error mounting hard drive."
		link_to_ramfs
		makeDir /data
		exit
	fi
	makeDir ${mountpoint}
done

# Create the file system directories
ln -s /data1 /data

logprint "Hard Drive Sucessfully Mounted."

# Run any custom script found in /data
if [ -r /data/custom.sh ] ;
then
   logprint "Running custom script found on hard drive."
   /bin/sh /data/custom.sh
fi


# DISK OK
/usr/bin/logger -p local6.notice "${DISKOK} : DISK OK"
echo ${DISKOK} > ${LEDFILE}


