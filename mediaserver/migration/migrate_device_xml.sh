#!/bin/sh


# Source common library
. /etc/scripts/common.rc

logprint "Retrieving configuration..."

# Upgrade Device.xml to the latest version if needed
#
backupconfig="true"
saveconfig="false"
while(true)
do
	${MIGRATIONSCRIPTS}/check_device_upgrade.sh
	result=$?
	case $result in
		0)
			break
			;;
		1)
			logprint "Error obtaining version"
			break
			;;
		2)
			if [ x"$backupconfig" = x"true" ]; then
				${MIGRATIONSCRIPTS}/backup_device_xml.sh
			fi
			logprint "Upgrading device xml backing up old revision"
			${MIGRATIONSCRIPTS}/upgrade_device_xml.sh
			upgrade_status=$?
			if [ x"$upgrade_status" = x"1" ]; then
				logprint "Upgrading device xml Failed"
				saveconfig="false"
				break;
			fi
			saveconfig="true"
			backupconfig="false"
			;;
		3)
			${MIGRATIONSCRIPTS}/backup_device_xml.sh
			${MIGRATIONSCRIPTS}/revert_device_xml.sh
			revert_status=$?
			if [ x"$revert_status" = x"1" ]; then
				logprint "revert device xml Failed"
				break;
			fi
			;;
	esac
done

if [ x"$saveconfig" = x"true" ]; then
	${MIGRATIONSCRIPTS}/finalize_device_upgrade.sh
fi
