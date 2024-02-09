#!/bin/sh

# Source common library
. /etc/scripts/common.rc

. /etc/scripts/database_management_variables.rc


cleanup() {
    /bin/rm -rf ${WORKAREA}
}

# Are we really on a dms??
if [ ! -f /etc/config/sw_profile_dms.txt ] && [ ! -f /etc/config/sw_profile_ms.txt ] ; then
    logerror "This does not seem to be a Management Server device."
    exit 1
fi


# Get the saved database name from command line
DBNAME=$1
if [ -z ${DBNAME} ] ; then
    list_databases
    showerror "Provide a name of the database snapshot to restore from."
    exit 1
fi

# Make sure that the database file exists
if [ ! -f ${DBREPOS}/${DBNAME}.tgz ]; then
    list_databases
    showerror "Database with name \"${DBNAME}\" not found."
    exit 1
fi

logprint "Restoring database from \"${DBNAME}\"."

  
# database import will be scheduled on next time stat up and will reboot. 
/etc/scripts/ipvsxmldbctl.sh schedule_importdb ${DBNAME}


 # Now reboot
logprint "Rebooting device..."
touch /tmp/reset
