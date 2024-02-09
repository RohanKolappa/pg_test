#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

get_device_type "$1"

load_hardware_profile

# Get netserver IP address
NETSERVER_IP="$2"
if [ x${NETSERVER_IP} = "x" ] ; then
    echo "ERROR: Need to specify IP address of Netperf server."
    exit 1
fi


if [ x$3 = "x" ] ; then
    echo "ERROR: Please provide Serial Number of device."
    exit 1
fi
SERIAL_NUMBER="$3"


# Run all the scripts
display_banner "Running \"check_device\""
/etc/scripts/check_device.sh ${DEVICE}
exit_on_error "Test script \"check_device\" failed to run sucessfully."

display_banner "Running \"network_io_test.sh\""
/etc/scripts/network_io_test.sh ${DEVICE} ${NETSERVER_IP}
exit_on_error "Test script \"network_io_test.sh\" failed to run sucessfully."

display_banner "Running \"disk_io_test.sh\""
/etc/scripts/disk_io_test.sh ${DEVICE}
exit_on_error "Test script \"disk_io_test.sh\" failed to run sucessfully."

display_banner "Running \"generate_test_report.sh\""
/etc/scripts/generate_test_report.sh ${SERIAL_NUMBER}
exit_on_error "Script \"generate_test_report.sh\" failed to run sucessfully."


