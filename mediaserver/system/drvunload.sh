#!/bin/sh

DRIVERNAME=fpga_module

# Check if driver currently loaded
lsmod | grep $DRIVERNAME > /dev/null 2>&1
if [ "x$?" != "x0" ]; then
   echo "Driver not currently loaded!"
   exit 0
fi


/sbin/rmmod $DRIVERNAME > /dev/null 2>&1
if [ "x$?" != "x0" ]; then
   echo "Could not unload driver!"
   echo "It might be currently in use."
   echo "Exit menu, StreamingServer, and retry."
   exit 1
fi

echo "Driver unloaded."
