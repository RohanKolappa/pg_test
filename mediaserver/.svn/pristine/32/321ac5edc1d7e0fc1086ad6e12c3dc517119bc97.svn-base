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
ver=`xsltproc ${XSL_DIR}/get_service_version.xsl ${CONFIG_DIR}/Device.xml`;

if [ "x$ver" = "x" ] ; then
	ver="1.0"
fi
logprint "Running ${SCRIPTS_DIR}/devices/${ver}/scripts/upgrade.sh"
${MIGRATIONSCRIPTS}/${ver}/device/scripts/upgrade.sh
newver=`xsltproc ${XSL_DIR}/get_service_version.xsl ${CONFIG_DIR}/Device.xml`;
if [ x"$newver" = "x" ] ; then
	logprint "Upgrade failed !!  Version $ver"
	exit 1
fi
# newver should be > ver
oldmajor=`echo ${ver} | cut -f1 -d'.'`
oldminor=`echo ${ver} | cut -f2 -d'.'`
oldrevision=`echo ${ver} | cut -f3 -d'.'`
if [ x"${oldrevision}" = "x" ]; then
	oldrevision=0
fi

newmajor=`echo ${newver} | cut -f1 -d'.'`
newminor=`echo ${newver} | cut -f2 -d'.'`
newrevision=`echo ${newver} | cut -f3 -d'.'`
if [ x"${newrevision}" = "x" ]; then
	newrevision=0
fi

if [ $oldmajor -gt $newmajor ] ; then
	logprint "$0 Upgrade failed Version roll back!!  Version $ver->$newver"
	exit 1
elif [ $oldmajor -eq $newmajor ] ; then
	if [ $oldminor -gt $newminor ] ; then
		logprint "$0 Upgrade failed Version roll back!!  Version $ver->$newver"
		exit 1
	elif [ $oldminor -eq $newminor ] ; then
		if [ $oldrevision -ge $newrevision ] ; then
			logprint "$0 Upgrade failed !!  Version $ver->$newver"
			exit 1
		fi
	fi
fi

exit 0

