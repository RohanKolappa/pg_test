#!/bin/sh

SCRIPTS_DIR=/etc/scripts

#SCRIPTS_DIR=.

# Source upgrade library
. ${SCRIPTS_DIR}/database_upgrade_variables.rc

# Source common library
. ${SCRIPTS_DIR}/common.rc


if [ ! -f ${XSL_DIR}/get_service_version.xsl ] ; then
    logerror "Missing file ${XSL_DIR}/get_service_version.xsl"
    exit 1
fi


# The Device.xml file better be there
if [ ! -f ${CONFIG_DIR}/Device.xml ] ; then
	logprint "Did not find Device.xml. Aborting..."
	exit 1
fi


current_version=`xsltproc ${XSL_DIR}/get_service_version.xsl ${CONFIG_DIR}/Device.xml`;

if [ "x$current_version" = "x" ] ; then
	current_version="1.0"
fi

# Save off the current Device.xml in a well known location in compact flas
logprint "Saving current config at version ${current_version} to old_configs in compact flash ..."
mkdir -p ${TMP_AREA}/files 
rm -rf ${TMP_AREA}/files/*
cp ${CONFIG_DIR}/Device.xml ${TMP_AREA}/files
tar -C ${TMP_AREA} -czf ${TMP_AREA}/version_${current_version}.tgz files
rm -rf ${TMP_AREA}/files
test -x /bin/cfmount && /bin/cfmount
mkdir -p ${CF_DIR}/old_configs
cp ${TMP_AREA}/version_${current_version}.tgz ${CF_DIR}/old_configs
test -x /bin/cfunmount && /bin/cfunmount
