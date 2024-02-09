#!/bin/sh

SCRIPTS_DIR=/etc/scripts

scripterror="1"
cfguptodate="0"
cfgold="2"
cfgnew="3"

#SCRIPTS_DIR=.

# Source upgrade library
. ${SCRIPTS_DIR}/database_upgrade_variables.rc

# Source common library
. ${SCRIPTS_DIR}/common.rc


# Function to retrieve an older compatible config 

# Determine target version
if [ ! -f ${XML_DIR}/service.version ] ; then
    logprint "Cannot determine target version. Aborting..."
    exit ${scripterror}
fi

if [ ! -f ${XSL_DIR}/get_service_version.xsl ] ; then
    logerror "Missing file ${XSL_DIR}/get_service_version.xsl"
    exit ${scripterror}
fi

target_version=`xsltproc ${XSL_DIR}/get_version.xsl ${XML_DIR}/service.version`;
TARGET_MAJOR_NUMBER=`echo ${target_version} | cut -f1 -d'.'`
TARGET_MINOR_NUMBER=`echo ${target_version} | cut -f2 -d'.'`
TARGET_REVISION_NUMBER=`echo ${target_version} | cut -f3 -d'.'`
if [ x"${TARGET_REVISION_NUMBER}" = "x" ]; then
	TARGET_REVISION_NUMBER=0
fi



# The Device.xml file better be there
if [ ! -f ${CONFIG_DIR}/Device.xml ] ; then
    logprint "Did not find Device.xml. Trying to see if we have any older config."
    get_old_config
    if [ ! -f ${CONFIG_DIR}/Device.xml ] ; then
        logprint "Did not find Device.xml. Aborting..."
        exit ${scripterror}
    fi
fi


# Get  current version of file
if [ ! -f ${XSL_DIR}/get_version.xsl ] ; then
    logerror "Missing file ${XSL_DIR}/get_version.xsl"
    exit ${scripterror}
fi
current_version=`xsltproc ${XSL_DIR}/get_service_version.xsl ${CONFIG_DIR}/Device.xml`;

if [ "x$current_version" = "x" ] ; then
	current_version="1.0"
fi

# Find starting point
CURRENT_MAJOR_NUMBER=`echo ${current_version} | cut -f1 -d'.'`
CURRENT_MINOR_NUMBER=`echo ${current_version} | cut -f2 -d'.'`
CURRENT_REVISION_NUMBER=`echo ${current_version} | cut -f3 -d'.'`
if [ x"${CURRENT_REVISION_NUMBER}" = "x" ]; then
	CURRENT_REVISION_NUMBER=0
fi

if [ "x$CURRENT_MAJOR_NUMBER" = "x" ] ; then
    logerror "Could not determine MAJOR number from current version."
    exit ${scripterror}
fi

if [ "x$CURRENT_MINOR_NUMBER" = "x" ] ; then
    logerror "Could not determine MINOR number from current version."
    exit ${scripterror}
fi

if [ "x$CURRENT_REVISION_NUMBER" = "x" ] ; then
    logerror "Could not determine REVISION number from current version."
    exit ${scripterror}
fi

# Do we need to upgrade Device.xml at all??
if [ x${current_version} = x${target_version} ] ; then
    logprint "Device.xml is upto date."
    exit ${cfguptodate}
fi

# Ok, we need to upgrade the Device.xml
logprint "$0 Need to upgrade Device.xml from ${current_version} --> ${target_version}"

# If current config is in the future, we have to revert to an
# older config first
old_config_needed=0
if [ ${CURRENT_MAJOR_NUMBER} -gt ${TARGET_MAJOR_NUMBER} ] ; then
    old_config_needed=1
fi

if [ ${CURRENT_MAJOR_NUMBER} -eq ${TARGET_MAJOR_NUMBER} ] ; then
    if [ ${CURRENT_MINOR_NUMBER} -gt ${TARGET_MINOR_NUMBER} ] ; then
        old_config_needed=1
    fi
fi
if [ ${CURRENT_MAJOR_NUMBER} -eq ${TARGET_MAJOR_NUMBER} ] ; then
    if [ ${CURRENT_MINOR_NUMBER} -eq ${TARGET_MINOR_NUMBER} ] ; then
		if [ ${CURRENT_REVISION_NUMBER} -gt ${TARGET_REVISION_NUMBER} ] ; then
			old_config_needed=1
		fi
    fi
fi

logprint "$0 Old config needed ${old_config_needed}"
if [ x${old_config_needed} = "x1" ] ; then
	exit ${cfgnew};
fi

exit ${cfgold};

