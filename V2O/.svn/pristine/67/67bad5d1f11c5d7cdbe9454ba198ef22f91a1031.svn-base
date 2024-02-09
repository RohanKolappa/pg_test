#!/bin/sh
if [ $# -lt 1 ] ; then
    echo "Usage: setboardinfoloop ITER"
    exit 0
fi
i=1
while [ $i -le $1 ]
do
   echo "Change board type to RX ($i iterations)"
   if [ -e /V2O/firmware/v2d_tx_lx80.bin ] ; then
       /bin/rm -f /V2O/firmware/v2d_rx_lx80.bin
   fi
   cp /mnt/cf/v2d_tx_lx80.bin /V2O/firmware/v2d_tx_lx80.bin 
   /V2O/bin/set_board_info -channelNumber 1 -boardtype StreamDst
   /V2O/bin/get_board_info -channelNumber 1 | grep "StreamDst"  > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo "Wrong board type (should be StreamDst) !!!"
       exit 0
   fi 
   /V2O/bin/readedideeprom -c 1 | grep "Raw EDID Data"  > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo "Unable to read EDID EEPROM !!!"
       exit 0
   fi 
   echo "Change board type to TX ($i iterations)"
   /V2O/bin/set_board_info -channelNumber 1 -boardtype StreamSrc
   /V2O/bin/get_board_info -channelNumber 1 | grep "StreamSrc" > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo "Wrong board type (should be StreamSrc) !!!"
       exit 0
   fi 
   /V2O/bin/readedideeprom -c 1 | grep "Raw EDID Data"  > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo "Unable to read EDID EEPROM !!!"
       exit 0
   fi 
   i=`expr $i + 1`
done
   
