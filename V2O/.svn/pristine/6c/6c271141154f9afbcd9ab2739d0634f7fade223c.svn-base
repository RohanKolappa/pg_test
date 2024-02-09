#!/bin/sh


IMAGEFILE=V2O_IMAGE

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
if /usr/bin/wget -O V2O_IMAGE ftp://${REMOTE}/${IMAGEFILE}
then
    # Write image to flash
    if /V2O/bin/mtdcp V2O_IMAGE 0
    then
	echo "Sucessfully upgraded software. Please reboot (Power off/Power on)."

	# Delete tmp files
	/bin/rm ${IMAGEFILE}

    else
	echo "*******************************************************************************"
	echo "Error: Failed software upgrade. Retry by repeating previous command."
	echo "Important: Please do not reboot (Power off/Power on) the system"
	echo "until software upgrade is complete!!"
	echo "*******************************************************************************"
    fi
else
    echo "*******************************************************************************"
    echo "Error: Could not download software image via anonymous ftp to ${REMOTE}."
    echo "Please check FTP server and confirm ${IMAGEFILE} can be successfully"
    echo "retrieved via anonymous FTP."
    echo "*******************************************************************************"

fi

