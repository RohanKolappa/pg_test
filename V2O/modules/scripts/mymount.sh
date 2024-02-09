#!/bin/sh

# Mount the NFS directory on server
/bin/mount -t nfs -o nolock,vers=2 10.10.2.135:/home/rahul/nfsdir /V2O

# Load the latest fpga module into the kernel
/usr/bin/load.sh

# Load initial LUTs
/V2O/bin/loadlut

# Apply network configuration
/V2O/bin/loadnet

# Apply route configuration
/V2O/bin/loadnet -r

