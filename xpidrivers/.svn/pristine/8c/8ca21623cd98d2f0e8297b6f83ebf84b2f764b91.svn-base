#!/bin/sh
/bin/cat /proc/modules | grep "^Barco_Adv7604" > /dev/null 2>&1
if [ $? == 0 ]; then
    /sbin/rmmod Barco_Adv7604
fi
/bin/cat /proc/modules | grep "^Barco_Adv7612" > /dev/null 2>&1
if [ $? == 0 ]; then
    /sbin/rmmod Barco_Adv7612
fi
/bin/cat /proc/modules | grep "^Barco_NetvizXpi" > /dev/null 2>&1
/bin/cat /proc/modules | grep "^Barco_Sii7172" > /dev/null 2>&1
if [ $? == 0 ]; then
    /sbin/rmmod Barco_Sii7172
fi
/bin/cat /proc/modules | grep "^Barco_NetvizXpi" > /dev/null 2>&1
if [ $? == 0 ]; then
    /sbin/rmmod Barco_NetvizXpi
fi
/bin/cat /proc/modules | grep "^pca954x" > /dev/null 2>&1
if [ $? == 0 ]; then
    /sbin/rmmod pca954x
fi
