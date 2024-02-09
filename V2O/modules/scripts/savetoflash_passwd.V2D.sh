#!/bin/sh

cd /tmp
mkdir configs_passwd

# Copy password and group files to the File System
/bin/cp /etc/passwd configs_passwd
/bin/cp /etc/group configs_passwd
/bin/cp /etc/shadow configs_passwd
/bin/cp /etc/gshadow configs_passwd

# Tar the config files
tar -czvf configs_passwd.tgz configs_passwd

# The flash must already have been mounted on /CF

# Copy the zipped files to flash
if [ -f /CF/bzImage ]   # make sure filesystem had linux kernel originally
then 
    sync_cp configs_passwd.tgz /CF/configs_passwd.tgz
	
    # Delete tmp files
    /bin/rm -rf configs_passwd.tgz configs_passwd
    logger "Saved password files to flash."
    exit 0
fi

logger "Error saving password files to flash."

exit 1


