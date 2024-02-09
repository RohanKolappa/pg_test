#!/bin/sh

. /etc/scripts/common.rc

# Sync and refuse to unmount
if [ -f /etc/config/arm_system ] ; then 
    sync
    echo "Ignored: Compact flash should remain mounted!!"
    exit 0
fi

# Check if compact flash is currently mounted
mount -t ${CF_FS_TYPE} | grep cf > /dev/null

if [ "x$?" != "x0" ]; then
   echo "Compact flash not currently mounted!!"
   exit 0
fi

# Sync and unmount
sync

umount /mnt/cf

if [ "x$?" != "x0" ]; then
   echo "Error: Failed unmounting compact flash."
   exit 1
fi

echo "Unmounted compact flash."
