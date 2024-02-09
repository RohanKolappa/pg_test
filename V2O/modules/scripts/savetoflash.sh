#!/bin/sh

IMAGEFILE=/tmp/config.img
MOUNTPOINT=/mnt

# Create initial image file for File System filled with zeros
/bin/dd if=/dev/zero of=$IMAGEFILE bs=128k count=4  > /dev/null 2>&1

# Create File System out of image file
/sbin/mkfs.minix $IMAGEFILE  > /dev/null 2>&1

# Mount the File System 
if /bin/mount -o loop -t minix  $IMAGEFILE $MOUNTPOINT  > /dev/null 2>&1
then

    # Copy config and private script files to the File System
    /bin/cp /V2O/config/*.txt $MOUNTPOINT
    /bin/cp /V2O/sbin/*.sh $MOUNTPOINT

    # Unmount the file system
    /bin/umount $MOUNTPOINT

    # Compress file system image file
    /bin/gzip  -c $IMAGEFILE > ${IMAGEFILE}.gz
    
    # Save the compressed image file to Flash.
    if /V2O/bin/mtdcp ${IMAGEFILE}.gz 0x300000
    then
	echo Saved config to Flash
	echo `date` > /tmp/success
    else
	echo Failed to save config to Flash
	echo `date` > /tmp/failed
	exit 1
    fi
else
    exit 1
fi

# Delete tmp files
/bin/rm ${IMAGEFILE} ${IMAGEFILE}.gz

