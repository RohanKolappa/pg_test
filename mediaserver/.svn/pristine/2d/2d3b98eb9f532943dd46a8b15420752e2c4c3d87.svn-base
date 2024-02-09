#!/bin/sh

# Source common library
. /etc/scripts/common.rc

FORCE_SAVE=0

# Save Device.xml to flash

# Temporary hack: Recreate Device.xml from current config
# TODO: Remove the temporary hack

# First call the dumpConfig command that will write all the 
# config to /tmp/configfile
UPGRADEDCONFIG_FILE=/etc/config/Device.xml
TMPCONFIG_FILE=/tmp/stage3.xml
create_transaction_log() {
	if [ x"${FORCE_SAVE}" = "x1" ] ; then
		/bin/rm -f ${CONFIG_FILE}
		/V2O/bin/dump_config
	fi

	# Make the /tmp/configfile look like one xml document rooted at <CLIConfigs>
	/bin/echo "<CLIConfigs>" > ${CONFIG_FILE}.save
	/bin/grep -v '<?xml version="1.0"' ${CONFIG_FILE} >> ${CONFIG_FILE}.save
	/bin/echo "</CLIConfigs>" >> ${CONFIG_FILE}.save
}
clear_transaction_log() {
		/bin/rm -f ${CONFIG_FILE}
		/bin/rm -f ${CONFIG_FILE}.save
}

add_list_entries() {
	local objectxmldir="/home/onair/objectXml/"
	local cmd=""
	local index=""
	local default=""
	local filename=""
	local tmplistentry="/tmp/listentry"
	local updateconfig="/etc/config/Device.xml"

	xsltproc -o /tmp/addlist.dat /etc/scripts/xsl/get_addlist.xsl ${CONFIG_FILE}.save
	xsltproc -o /tmp/listdefault.dat /etc/scripts/xsl/get_listfile.xsl /home/onair/objectXml/confighandler.xml
	for i in `cat /tmp/addlist.dat`
	do
		cmd=`echo $i | cut -f2 -d':'`
		index=`echo $i | cut -f3 -d':'`
		default=`grep $cmd /tmp/listdefault.dat | cut -f3 -d':'`
		filename=${objectxmldir}`basename ${default}`
		sed -e 's/ListIndex="1"/'ListIndex=\"$index\"/ $filename > ${tmplistentry}
		xsltproc -o /tmp/tmp.xml --stringparam filename ${tmplistentry}  --stringparam cmdname $cmd /etc/scripts/xsl/add_list.xsl ${UPGRADEDCONFIG_FILE}
		cp /tmp/tmp.xml ${UPGRADEDCONFIG_FILE}
	done
}
save_configuration() {
	# Now call the java config converter code to create the Device.xml file
	AGENT=`get_agent_app_name`

	if [ ! -f /tmp/stage1.xsl ]; then
		xsltproc -o /tmp/stage1.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/devicesaveconfig.xsl  /home/onair/objectXml/confighandler.xml
	fi
	sed -ie 's/mod_/add_/' ${CONFIG_FILE}.save
	xsltproc /tmp/stage1.xsl ${CONFIG_FILE}.save | sed -e 's/No_BOOL_/false/g' | sed -e 's/Yes_BOOL_/true/g' | sed -e 's/Both_BOOL_/Both/g'  > /tmp/stage2.xsl
	xsltproc -o ${TMPCONFIG_FILE} /tmp/stage2.xsl ${UPGRADEDCONFIG_FILE}

	# Remove cache of CLI commands that fire on startup
	/bin/rm -f /mnt/cf/apply.sh

	# Remove factory configuration used to startup wizard
	/bin/rm -f /mnt/cf/factory

	/bin/cp ${TMPCONFIG_FILE} ${UPGRADEDCONFIG_FILE}
	/bin/cp ${TMPCONFIG_FILE} /mnt/cf/Device.xml.new
	/bin/mv /mnt/cf/Device.xml.new /mnt/cf/Device.xml
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

	#for change board type
	if [ -f /etc/config/arm_system ] ; then
		/bin/rm -f /mnt/cf/sys_type_xpi_encoder
		/bin/rm -f /mnt/cf/sys_type_xpi_decoder
		/bin/cp /etc/config/sys_type_xpi_* /mnt/cf/
	fi


	# Tar the config files
	tar -czf configs.tgz configs

	# The flash must already have been mounted on /CF
	/V2O/bin/sync_cp configs.tgz /mnt/cf/configs.tgz
	
	# Copy cache files
	/bin/rm -f /mnt/cf/cache
	/bin/mkdir -p /mnt/cf/cache
	/bin/cp configs/*cache.txt /mnt/cf/cache
	
	# Delete tmp files
	/bin/rm -rf configs.tgz configs
	
	# Unmount the flash
	/bin/cfunmount
	logprint "Saved configuration to flash."
}
print_usage() {
    echo "Usage: $0 force-save"
}


if [ x"$1" = "xforce-save" ] ; then
    FORCE_SAVE=1
elif [ x"$1" != "x" ] ; then
    print_usage
    exit 1
fi

create_transaction_log
add_list_entries
save_configuration
clear_transaction_log
exit 0;
