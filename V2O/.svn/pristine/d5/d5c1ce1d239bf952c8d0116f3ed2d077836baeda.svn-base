#!/bin/sh

# Create FPGA device file if needed

if [ ! -e /dev/fpga ]
then
  mknod /dev/fpga c 60 0
fi

if [ ! -e /dev/fpga_kb ]
then
  mknod /dev/fpga_kb c 60 1
fi

if [ ! -e /dev/fpga_ms ]
then
  mknod /dev/fpga_ms c 60 2
fi

# Unload module if already present
if grep -q fpga /proc/modules
then
    rmmod fpga_module
fi


module=/V2D/modules/fpga_module.o
if [ -e $module ]
then
  echo "Loading FPGA module"  
  insmod -v $module > /dev/null
else
  echo "Module Not Found"
fi
