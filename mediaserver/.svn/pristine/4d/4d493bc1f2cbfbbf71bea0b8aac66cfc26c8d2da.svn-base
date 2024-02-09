#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

get_device_type "$1"

load_hardware_profile

/etc/scripts/diskutils.sh init ${DEVICE}
