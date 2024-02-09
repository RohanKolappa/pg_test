#!/bin/sh

SCRIPTS_DIR=/etc/scripts

#SCRIPTS_DIR=.

# Source upgrade library
. ${SCRIPTS_DIR}/database_upgrade_variables.rc

# Source common library
. ${SCRIPTS_DIR}/common.rc


# Write Device.xml back to compact flash
logprint "Saving upgraded Device.xml to compact flash..."
test -x /bin/cfmount && /bin/cfmount
cp ${CONFIG_DIR}/Device.xml ${CF_DIR}/
test -x /bin/cfunmount && /bin/cfunmount
