#!/bin/sh

# Set remote URL 
if [ $1 ]
then
    REMOTE=$1
else
    /bin/echo "Error: Provide URL for software image."
    /bin/echo " "
    /bin/echo "Usage: upgrade_software URL."
    exit
fi

# Change to /tmp directory
cd /tmp

# Download image using FTP
if /usr/bin/wget -O /var/V2D_IMAGE ${REMOTE}
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
    echo "Error: Could not download software image via HTTP to ${REMOTE}."
    echo "Please check URL and confirm software image can be successfully"
    echo "retrieved via HTTP."
    echo "*******************************************************************************"

fi

