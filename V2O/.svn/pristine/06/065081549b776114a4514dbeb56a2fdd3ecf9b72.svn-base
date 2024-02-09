#!/bin/sh 
if [ $# -lt 1 ] ; then
    echo "Usage: loadedideepromloop.sh ITER"
    exit 0
fi
i=1
while [ $i -le $1 ]
do
    echo "Loading EDID EEPROM on Channel 1 $i times"
    /V2O/bin/loadedideeprom -c 1
    /V2O/bin/readedideeprom -c 1 | grep "Raw EDID Data" > /dev/null 2>&1
    if [ x$? != "x0" ] ; then
        echo "Unable to read EDID EEPROM!!!"
    fi
    i=`expr $i + 1`
done
