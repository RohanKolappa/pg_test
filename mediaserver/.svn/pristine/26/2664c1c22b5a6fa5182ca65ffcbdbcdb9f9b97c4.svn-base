#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

get_device_type "$1"

load_hardware_profile

if [ x$RAID_CONTROLLER_EXISTS != "x1" ] ; then 
    echo
    echo "Upgrading RAID firmware not applicable on this device. "
    echo
fi

FIRMWARE_IMAGE=prom0006.img
if [ x${RAID_CONTROLLER_FIRMWARE_IMAGE} != "x" ] ; then 
    FIRMWARE_IMAGE=${RAID_CONTROLLER_FIRMWARE_IMAGE}
fi

# Setup variables
TWHOME=/bin

# Update the Bios firmware
echo
echo "Upgrading firmware on RAID device. Please wait till done..."
echo

echo "y" > /tmp/yes.txt
${TWHOME}/tw_cli update fw=/home/onair/maintutils/firmware/${FIRMWARE_IMAGE} < /tmp/yes.txt > /dev/null

echo
echo "Please reboot system for the firmware update to take effect."
echo
