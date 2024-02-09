#!/bin/sh

# Assign IP adresses for the target
# echo Assigning IP address ...
# /sbin/ifconfig eth0 10.10.2.221 netmask 255.255.255.0
# sleep 1

# Mount the NFS directory on server
echo Mounting NFS directory ...
if /bin/mount -t nfs -o nolock,vers=2 10.10.2.135:/home/rahul/V2DR /V2O
then
    echo "Sucessfully mounted NFS file system on V2DR."
else
   if /bin/mount -t nfs -o nolock,vers=2 10.10.2.135:/home/rahul/V2DT /V2O
   then
       echo "Sucessfully mounted NFS file system on V2DT."
   else
      echo "Could not mount NFS file system."
      exit 1
   fi
fi

# Load the latest fpga module into the kernel
/usr/bin/load.sh

# Load initial LUTs
/V2O/bin/loadlut

# Apply network configuration
/V2O/bin/loadnet

# Apply route configuration
/V2O/bin/loadnet -r

