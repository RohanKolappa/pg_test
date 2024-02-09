#!/bin/sh

# Source common library
. /etc/scripts/common.rc

. /etc/scripts/database_management_variables.rc


cleanup() {
    /bin/rm -rf ${WORKAREA}
}

# Are we really on a dms??
if [ ! -f /etc/config/sw_profile_dms.txt ] ; then
    logerror "This does not seem to be a Management Server device."
    exit 1
fi


# Get the database name from command line
DBNAME=$1
if [ -z ${DBNAME} ] ; then
    showerror "Provide a name to be assigned to the current database snapshot."
    exit 1
fi

# Create directory if it does not exist
mkdir -p ${DBREPOS}

# Make sure that we are not overwriting an existing snapshot
if [ -f "${DBREPOS}/${DBNAME}.tgz" ]; then
    echo 
    echo "Database with name \"${DBNAME}\" already exists."

    echo
    read -p "Overwite [y/n] ? " result; echo

    if [ x"$result" != "xy" ]; then
        echo "Aborted."
        exit 
    fi
fi

logprint "Saving current database as \"${DBNAME}\""

/etc/scripts/ipvsxmldbctl.sh export ${DBNAME}



