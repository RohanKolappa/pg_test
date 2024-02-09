#!/bin/sh

IMAGEFILE=/tmp/config.img
MOUNTPOINT=/mnt

cd /tmp

# Note the flash must already have been mounted on /CF

# Retrieve compressed image file from Flash
if tar -xzvf /CF/configs.tgz
then
    logger -s "Found Configuration in Flash."
    # Copy config files to V2O/config
    /bin/cp configs/*.txt /V2O/config

    # Copy script files to V2O/sbin
    /bin/cp configs/*.sh /V2O/sbin

    # Copy ssh files to V2O/config
    /bin/cp configs/sshd_config /V2O/config

    # Copy ssh keys to /usr/local/etc
    /bin/cp configs/*_key /usr/local/etc
    /bin/cp configs/*_key.pub /usr/local/etc
else
    logger -s "Error: Could not retrieve old configuration from flash."
    logger -s "Using Defaults"
fi

# Delete tmp files
/bin/rm -rf configs
