#!/bin/sh



# Set remote IP address
if [ $1 ]
then
    REMOTE=$1
else
    /bin/echo "Error: Provide FTP server IP address."
    /bin/echo " "
    /bin/echo "Usage: software_update.sh FTPSERVER."
    exit
fi

# Change to /tmp directory
cd /tmp

# Download image using FTP
if /usr/bin/ftpget ${REMOTE} . fpga_module.o 
then
    /usr/bin/ftpget ${REMOTE} . vutils

    killall menu > /dev/null 2>&1
    killall dserver > /dev/null 2>&1
    killall dclient > /dev/null 2>&1

    sleep 1

    # Copy files to respective locations
    /bin/cp -f fpga_module.o /V2O/modules                        
    /bin/cp -f vutils /V2O/bin

    # Remove temp files
    /bin/rm fpga_module.o vutils

    # Restart services
    /usr/bin/load.sh
    /V2O/bin/loadlut
    /V2O/bin/loadnet -v

    echo "Software files updated. (Not permanent!!)."
else
    echo "*******************************************************************************"
    echo "Error: Could not  software image via anonymous ftp to ${REMOTE}."
    echo "Please check FTP server and confirm fpga_module.o can be successfully"
    echo "retrieved via anonymous FTP."
    echo "*******************************************************************************"

fi
