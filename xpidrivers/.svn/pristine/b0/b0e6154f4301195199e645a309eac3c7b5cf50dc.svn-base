#!/bin/sh
/bin/cat /proc/modules | grep "^OmniTek_Driver" > /dev/null 2>&1
if [ $? != 0 ]; then
    /sbin/insmod /V2O/modules/OmniTek_Driver.ko
fi
/bin/cat /proc/modules | grep "^Barco_Adv7604" > /dev/null 2>&1
if [ $? != 0 ]; then
    /sbin/insmod /V2O/modules/Barco_Adv7604.ko
fi
/bin/cat /proc/modules | grep "^Barco_Sii7172" > /dev/null 2>&1
if [ $? != 0 ]; then
    /sbin/insmod /V2O/modules/Barco_Sii7172.ko
fi
/bin/cat /proc/modules | grep "^Barco_Adv7612" > /dev/null 2>&1
if [ $? != 0 ]; then
    /sbin/insmod /V2O/modules/Barco_Adv7612.ko
fi
/bin/cat /proc/modules | grep "^Barco_NetvizXpi" > /dev/null 2>&1
if [ $? != 0 ]; then
    /sbin/insmod /V2O/modules/Barco_NetvizXpi.ko
fi
