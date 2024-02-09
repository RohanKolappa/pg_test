#!/bin/sh


# Source common library
. /etc/scripts/common.rc

#
# Setup Operating System (kernel)
#
${SCRIPTS}/setup_kernel.sh

#
# Start all essential system services available in the base OS.
# These can be started before loading compact flash since they
# do not depend on any application images (/home/onair)
#
${SCRIPTS}/start_base_services.sh


#
# Retrieve and install all our application images from the compact flash
#
if [ ! -f /etc/config/arm_system ] ; then 
    ${SCRIPTS}/load_application_images.sh
else

    # If compact flash has our custom hook, run it
    if [ -r /mnt/cf/custom_startup.sh ] ; then
      logprint "Found Custom Startup Script"
      /bin/sh /mnt/cf/custom_startup.sh
    fi
fi

#
# If compact flash not found, or our software not found on the
# compact flash (onairapp.tgz) we cannot do much else.
# We exit at this point and let this be just a simple linux system
#
if [ -f /tmp/cf_not_found ]; then
    echo "Could not load system software from compact flash!!"
    echo "Assigning factory default IP addresses."
    /sbin/ifconfig eth0 192.168.1.1 netmask 255.255.255.0
    /sbin/ifconfig eth1 10.1.5.254 netmask 255.255.255.0
    /sbin/route add -net default gw 192.168.1.254
    exit
fi

#
# Now that /home/onair is avaliable
# setup the file system appropriately (create links, directories, etc)
#
${SCRIPTS}/setup_file_system.sh


#
# Retrieve configuration from the compact flash
#
${SCRIPTS}/get_config.sh

#
# Mount the hard drive (if available) on /data 
#
${SCRIPTS}/mount_hard_drive.sh

#
# Now that /data is avaliable, setup the files on that as needed
#
${SCRIPTS}/setup_data_file_system.sh

#
# Now start all the daemons that reside in /home/onair
# but do not depend on the apply process
#
${SCRIPTS}/start_system_services.sh

#
# Apply the configuration
#
${SCRIPTS}/apply_config.sh

#
# Now start all the daemons that depend on the apply
#
${SCRIPTS}/start_application_services.sh

#
# We are done.
#
logprint "Server Ready!"


# STREAM OK
/usr/bin/logger -p local6.notice "${STREAMOK} : STREAM OK"
echo ${STREAMOK} > ${LEDFILE}

# Save bootup messages in a more permanent file
# Warning: Do not change the filename. See DRY.1
/bin/cp /var/log/messages /var/log/boot_messages




