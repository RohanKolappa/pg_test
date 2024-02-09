#!/bin/sh

cd /tmp
mkdir configs

# Copy config and private script files to the File System
/bin/cp /V2O/config/*.txt configs
/bin/cp /V2O/config/sshd_config configs
/bin/cp /V2O/sbin/*.sh configs
/bin/cp /usr/local/etc/*_key configs
/bin/cp /usr/local/etc/*_key.pub configs
/bin/cp /V2O/config/*.png configs

# Tar the config files
tar -czvf configs.tgz configs

#mount the flash
/bin/mount -t vfat /dev/hda1 /mnt/cf

# The flash must already have been mounted on /CF
if [ -f /mnt/cf/syslinux.cfg ]   # make sure filesystem had linux kernel originally
then 
    /V2O/bin/sync_cp configs.tgz /mnt/cf/configs.tgz
	
    # Delete tmp files
    /bin/rm -rf configs.tgz configs
    #unmount the flash
    /bin/umount /mnt/cf
    logger "Saved configuration to flash."
    exit 0
fi

logger "Error saving configuration to flash."

exit 1


