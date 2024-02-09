#!/bin/sh

IMAGEFILE=/tmp/config.img
MOUNTPOINT=/mnt


# Retrieve compressed image file from Flash
/bin/dd if=/dev/mtd0 of=${IMAGEFILE}.gz bs=128k count=1 skip=24 > /dev/null 2>&1

# Decompress image file
if /bin/gzip  -c -d ${IMAGEFILE}.gz > ${IMAGEFILE}
then
    # Mount the image file
    if /bin/mount -o loop -t minix  ${IMAGEFILE} $MOUNTPOINT
    then
	echo "Found File System in Flash."
	# Copy config files to V2O/config
	/bin/cp ${MOUNTPOINT}/*.txt /V2O/config

	# Copy script files to V2O/sbin
	/bin/cp ${MOUNTPOINT}/*.sh /V2O/sbin

	# Unmount the file system
	/bin/umount $MOUNTPOINT
    fi
else
    echo "No File System in Flash."
fi

# Delete tmp files
/bin/rm ${IMAGEFILE} ${IMAGEFILE}.gz
