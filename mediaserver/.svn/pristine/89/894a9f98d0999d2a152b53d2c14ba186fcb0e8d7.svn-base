#!/bin/sh
. /etc/scripts/common.rc


if [  -f /etc/config/arm_system ] ; then
   echo "Compact flash should be permanently mounted!"
   exit 0
fi

# Make sure mount point and device exist
if [ ! -d /mnt/cf ]
then
    echo "Error: Directory /mnt/cf does not exist."
    exit 1
fi

if [ ! -b /dev/CF ]; then
    echo "Error: Compact Flash device not set"
    echo "Determine the device and create symbolic link to /dev/CF"
    echo "Example: ln -s /dev/hda1 /dev/CF"
    exit 1
fi

# Check if already mounted
mount -t ${CF_FS_TYPE} | grep cf > /dev/null

if [ "x$?" = "x0" ]; then
   echo "Compact flash already mounted!"
   exit 0
fi

# Mount it
mount -t ${CF_FS_TYPE} /dev/CF /mnt/cf

if [ "x$?" != "x0" ]; then
   echo "Error: Cannot mount the compact flash."
   exit 1
fi

echo "Compact flash mounted."


