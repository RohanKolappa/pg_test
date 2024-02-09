#!/bin/sh

# Identify the device on which the compact flash might be
ln -s /dev/hdd1 /dev/cf   # Assume this one by default

if [ -f /proc/ide/hdc/identify ]
then
    ln -f -s /dev/hdc1 /dev/cf
fi

if [ -f /proc/ide/hdb/identify ]
then
    ln -f -s /dev/hdb1 /dev/cf
fi

if [ -f /proc/ide/hda/identify ]
then
    ln -f -s /dev/hda1 /dev/cf
fi

# Mount the file system 
if /bin/mount -t vfat /dev/cf /CF
then
    logger "Flash device mounted on /CF"
else
    logger "Error mounting flash"
fi

exit 1
