#!/bin/sh

IMAGEFILE=V2D_IMAGE

# Set remote IP address
if [ $1 ]
then
    REMOTE=$1
else
    /bin/echo "Error: Provide FTP server IP address."
    /bin/echo " "
    /bin/echo "Usage: upgrade_software FTPSERVER  [IMAGE_FILE]."
    exit
fi

# Set build
if [ $2 ]
then
IMAGEFILE=$2
fi

# Change to /tmp directory
cd /tmp

# Download image using FTP
if /usr/bin/wget -O /var/V2D_IMAGE ftp://${REMOTE}/${IMAGEFILE}
then
    if [ -f /CF/bzImage ]   # make sure filesystem had linux kernel originally
    then 
        echo "Exploding image..."
        tar -xvf /var/V2D_IMAGE
        /V2O/bin/sync_cp image/bzImage /CF/bzImage
        /V2O/bin/sync_cp image/rootfs.gz /CF/rootfs.gz
        /V2O/bin/sync_cp image/syslinux.cfg /CF/syslinux.cfg

        # Delete tmp files
        # /bin/rm -rf image
	    
 	echo "Successfully upgraded software. Please reboot (or poweroff)."
    fi
else
    echo "*******************************************************************************"
    echo "Error: Could not download software image via anonymous ftp to ${REMOTE}."
    echo "Please check FTP server and confirm ${IMAGEFILE} can be successfully"
    echo "retrieved via anonymous FTP."
    echo "*******************************************************************************"

fi

