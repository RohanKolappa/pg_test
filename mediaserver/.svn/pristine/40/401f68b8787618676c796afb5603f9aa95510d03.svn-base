#!/bin/sh


echo "Mounting compact flash......"
/bin/cfmount
if [ -f /mnt/cf/bzImage -a -f /mnt/cf/syslinux.v2d_2_2 ] ; then
    echo "Reverting V2D device to previous 2.2 version..."
    /bin/cp /mnt/cf/syslinux.cfg /mnt/cf/syslinux.v2d_3_0
    /bin/cp /mnt/cf/syslinux.v2d_2_2 /mnt/cf/syslinux.cfg
    /bin/rm -f /mnt/cf/manifest.txt
    /bin/rm -f /mnt/cf/Device.xml
    /bin/cfunmount
    echo "The system should reboot automatically..."
    touch /tmp/reset
else
    echo "No V2D image found to revert to"
    echo "This does not seem to be an upgraded V2D 2.2 device"
    cfunmount
fi

