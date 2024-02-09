#!/bin/sh

   VIDINPUT=`/V2O/bin/get_edid_videoinput |grep videoinput |sed 's/.*value="\(.*\)".*/\1/'`
   if [ x${VIDINPUT} != x"Analog" ] ; then 
    /V2O/bin/loadedideeprom -c 1 
   else
    /V2O/bin/loadedideeprom -c 1 -A 0
   fi
