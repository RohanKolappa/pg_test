#!/bin/sh

IMAGEFILE=V2D_IMAGE

# Set remote user ID 
if [ $1 ]
then
    REMOTEUSER=$1
else
    /bin/echo "Error: Provide remote User name."
    /bin/echo " "
    /bin/echo "Usage: secure_upgrade_software USERNAME FTPSERVER [IMAGEFILE]."
    exit
fi

# Set remote IP address
if [ $2 ]
then
    REMOTEIP=$2
else
    /bin/echo "Error: Provide FTP server IP address."
    /bin/echo " "
    /bin/echo "Usage: secure_upgrade_software USERNAME FTPSERVER [IMAGEFILE]."
    exit
fi

# Set build
if [ $3 ]
then
IMAGEFILE=$3
fi

# Change to /tmp directory
cd /tmp

# Download image using FTP
if /usr/bin/scp ${REMOTEUSER}@${REMOTEIP}:${IMAGEFILE} /var/V2D_IMAGE
then
    if [ -f /CF/bzImage ]   # make sure filesystem had linux kernel originally
    then 
        echo "Exploding image..."
        tar -xvf /var/V2D_IMAGE
        echo "Copying files..."
        /V2O/bin/sync_cp image/bzImage /CF/bzImage
        /V2O/bin/sync_cp image/rootfs.gz /CF/rootfs.gz
        /V2O/bin/sync_cp image/syslinux.cfg /CF/syslinux.cfg


        # Delete tmp files
        # /bin/rm -rf image
	    
 	echo "Successfully upgraded software. Please reboot (or poweroff)."
    fi
else
    echo "*******************************************************************************"
    echo "Error: Could not retrieve software image via Secure Copy to ${REMOTE}."
    echo "Please check SCP server and confirm ${IMAGEFILE} can be successfully"
    echo "retrieved via SCP."
    echo "*******************************************************************************"

fi

