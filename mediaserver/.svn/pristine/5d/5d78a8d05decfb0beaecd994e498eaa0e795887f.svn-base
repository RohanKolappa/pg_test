#!/bin/sh

DRIVERNAME=fpga_module
DRIVERFILE=/V2O/modules/${DRIVERNAME}.ko

if [ ! -f ${DRIVERFILE} ] ; then
    echo "Error: driver file ${DRIVER} not found."
    exit 1
fi

# Check if driver currently unloaded
lsmod | grep $DRIVERNAME > /dev/null 2>&1
if [ "x$?" = "x0" ]; then
   echo "Driver already loaded!"
   exit 0
fi

DRIVERPARAMS=""
VIRTUALPARAMS=""
PCISLOTPARAMS=""
if [ -f /tmp/virtualboards ] ; then
    VIRTUALPARAMS=`cat /tmp/virtualboards`
fi

if [ -f /tmp/boardpcislots ] ; then
    PCISLOTPARAMS=`cat /tmp/boardpcislots`
fi

/sbin/insmod $DRIVERFILE $DRIVERPARAMS ${VIRTUALPARAMS} ${PCISLOTPARAMS}
echo "Driver loaded."
