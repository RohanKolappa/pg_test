#!/bin/sh

cd /tmp

# The flash must already have been mounted on /CF

# Retrieve compressed image file from Flash
if tar -xzvf /CF/configs_passwd.tgz
then
    logger -s "Found Password files in Flash."

    # Copy files to /etc
    /bin/cp configs_passwd/* /etc
else
    logger -s "Error: Could not retrieve old passwords from flash."
    logger -s "Using Defaults"
fi

# Delete old user home directories 
/bin/rm -rf /home/*

# Create new user home directories
/V2O/bin/mkudirs

# Delete tmp files
/bin/rm -rf configs_passwd
