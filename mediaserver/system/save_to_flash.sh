#!/bin/sh

# Source common library
. /etc/scripts/common.rc

# Save Device.xml to flash

# Temporary hack: Recreate Device.xml from current config
# TODO: Remove the temporary hack

# First call the dumpConfig command that will write all the 
# config to /tmp/configfile
REMOTE_CONFIG=/etc/config/remote_config.xml
CONFIG_FILE=/tmp/configfile
UPGRADEDCONFIG_FILE=/tmp/DeviceTmp.xml
TMPCONFIG_FILE=/tmp/stage3.xml
/bin/cp ${REMOTE_CONFIG} ${REMOTE_CONFIG}.save
sed -i -e 's/<Use>.*<\/Use>/<Use>File<\/Use>/' ${REMOTE_CONFIG}
sed -i -e 's/<Mode>.*<\/Mode>/<Mode>append<\/Mode>/' ${REMOTE_CONFIG}
/bin/rm -f ${CONFIG_FILE}
/V2O/bin/dump_config
/bin/mv ${REMOTE_CONFIG}.save ${REMOTE_CONFIG}

# Make the /tmp/configfile look like one xml document rooted at <CLIConfigs>
/bin/echo "<CLIConfigs>" > ${CONFIG_FILE}.save
/bin/grep -v '<?xml version="1.0"' ${CONFIG_FILE} >> ${CONFIG_FILE}.save
/bin/echo "</CLIConfigs>" >> ${CONFIG_FILE}.save
/bin/mv ${CONFIG_FILE}.save ${CONFIG_FILE}

# Now call the java config converter code to create the Device.xml file
# Call the xsl based apply only for the I50's
# Call Java based Apply for all other devices
AGENT=`get_device_interface_agent_name`
devicename=`cat /etc/config/hw_profile*`
	case "${AGENT}" in
		v2dtx|v2drx)
            /bin/cp /home/onair/objectXml/${devicename}.xml  ${UPGRADEDCONFIG_FILE}
            banner=`banner_get | urldecoder`
			xsltproc -o /tmp/stage1.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` --stringparam banner_str "${banner}"  /etc/scripts/xsl/devicesaveconfig.xsl  /home/onair/objectXml/confighandler.xml
			xsltproc -o /tmp/stage2.xsl /tmp/stage1.xsl ${CONFIG_FILE}
			sed -i 's/No_BOOL_/false/g;s/Yes_BOOL_/true/g' /tmp/stage2.xsl
			xsltproc -o ${TMPCONFIG_FILE} /tmp/stage2.xsl ${UPGRADEDCONFIG_FILE}
			/bin/cp  ${TMPCONFIG_FILE} ${UPGRADEDCONFIG_FILE}
			;;
		*)
			AGENT=`get_agent_app_name`
			java -cp /home/onair/modules/mediaroom/public/mediaroom.jar com.ipvs.device/${AGENT} UpgradeConfig ${CONFIG_FILE} ${UPGRADEDCONFIG_FILE} > /var/log/deviceapp/save_to_flash.log 2>&1
	esac


# Now that we have created our first Device.xml file, save it to flash
/bin/cfmount
/bin/cp ${UPGRADEDCONFIG_FILE} /mnt/cf/Device.xml
/bin/cfunmount

cd /tmp
mkdir configs

# Copy config and private script files to the File System
/bin/cp /V2O/config/*.txt configs
/bin/cp /V2O/config/sshd_config configs
/bin/cp /V2O/scripts/*.script configs
/bin/cp /usr/local/etc/*_key configs
/bin/cp /usr/local/etc/*_key.pub configs
/bin/cp /V2O/config/*.png configs

/bin/rm configs/*SKIP.txt

# Tar the config files
tar -czvf configs.tgz configs

#mount the flash
/bin/cfmount

# The flash must already have been mounted on /CF
/V2O/bin/sync_cp configs.tgz /mnt/cf/configs.tgz

# Copy cache files
/bin/mkdir -p /mnt/cf/cache
/bin/cp configs/*cache.txt /mnt/cf/cache

# Remove marker in compact flash that detects factory default at startup
/bin/rm -f /mnt/cf/factory
/bin/rm -f /etc/config/factory

# Delete tmp files
/bin/rm -rf configs.tgz configs

# Unmount the flash
/bin/cfunmount
logprint "Saved configuration to flash."
exit 0


