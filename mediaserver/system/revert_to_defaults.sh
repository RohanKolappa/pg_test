#!/bin/sh


# Source common library
. /etc/scripts/common.rc

# By default, we keep the network config
RESET_NETWORK=0
DELETE_MEDIA=0
TMPCONFIG_FILE=/tmp/stage3.xml

# Routine to stop all services before we start touching config files
stop_services() {

    # Stop all services
    if [  -f /etc/config/sw_profile_dms.txt ] || [  -f /etc/config/sw_profile_ms.txt ] ; then
				
        # Stop all HA services
        if [ -f /etc/config/hw_profile_ms.txt ] ; then
            logprint "Terminating ha"
            /etc/scripts/watchdog/watch_linuxha.sh manual
            /bin/sh /etc/scripts/watchdog/watch_linuxha.sh stop > /dev/null 2>&1
            /bin/sleep 3
        fi				

        # Make sure watch dog does not restart them 
        logprint "Terminating magamentserver"
        /etc/scripts/watchdog/watch_managementserver.sh manual
        /etc/scripts/watchdog/watch_managementserver.sh stop > /dev/null 2>&1
        /bin/sleep 3
				
        # Call individual server stop, if they were started in manual mode 
        logprint "Terminating mscomponent"
        /home/onair/mscomponent/bin/mscomponent.sh stop > /dev/null 2>&1
        /bin/sleep 3

        logprint "Terminating IpvsComponent"
        /home/onair/ipvscomponent/bin/ipvscomponent.sh stop > /dev/null 2>&1
        /bin/sleep 3

        logprint "Terminating Openfire"
        /etc/scripts/openfirectl.sh stop > /dev/null 2>&1
        /bin/sleep 3

        logprint "Terminating IPVS-XML-DB"
        /etc/scripts/ipvsxmldbctl.sh stop > /dev/null 2>&1
        /bin/sleep 3

        sync
    fi


    logprint "Terminating Streaming Server"
    /etc/scripts/watchdog/watch_streamingserver.sh stop

    logprint "Terminating NTP"
    /etc/scripts/watchdog/watch_ntp.sh stop
    
}

# Reset openfire database to factory state
reset_openfire_defaults() {
    # If MS or MSonDMS
    if [  -f /etc/config/sw_profile_dms.txt ] || [  -f /etc/config/sw_profile_ms.txt ] ; then
        # Delete temp folders used for import/export file and attach/detach dir
        logprint "deleting  import/export mediafile temp files /data/mediafile"
        /bin/rm -rf /data/mediafile
        logprint "deleting  import/export mediadir temp files /data/mediadir"
        /bin/rm -rf /data/mediadir
        logprint "deleting import/export media temp files /data/dms"
        /bin/rm -rf /data/dms
         logprint "deleting live thumbnail cache"
        /bin/rm -rf /data/tncache/live/*
         logprint "deleting recording thumbnail cache"
        /bin/rm -rf /data/tncache/recordings/*
         logprint "deleting demo files"
        /etc/scripts/demo_setup.sh teardown
        
        # Delete all backup db
        logprint "deleting saved database /data/saved_databases"
        /bin/rm -rf /data/saved_databases
        logprint "deleting snapshots /data/sedna_dbsnapshot"
        /bin/rm -rf /data/sedna_dbsnapshot
        logprint "deleting db migration temp files /data/dbmigration"
        /bin/rm -rf /data/dbmigration

        /etc/scripts/watchdog/watch_managementserver.sh reset
        

        if [ -f /etc/config/hw_profile_ms.txt ] ; then
            /bin/sh /etc/scripts/watchdog/watch_linuxha.sh reset
        fi
    fi
}

# Cleanup all media files
clean_media_files() {

    # Delete all media files
    if [ x"$DELETE_MEDIA" = "x1" ] ; then    
        logprint "Deleting all media files..."
        /bin/rm -rf /data/DSS/data/portal/site/default/*         
        /bin/rm -rf /data2/DSS/data/portal/site/default/*         

        if [ -f /etc/config/grub_system ] ; then 
		    devicename=`xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml`
            if [ $devicename = "DMS-1550-Device" ]; then 
                /bin/rm -rf /data3/DSS/data/portal/site/default/*         
                /bin/rm -rf /data4/DSS/data/portal/site/default/*         
            fi
        fi
    fi
}

# Reset EEPROM to factory state
# If CLEAR_EEPROM=0, retain EEPROM contents
clear_eeprom() {

    if [ -f /etc/config/hw_profile_v2dxp200.txt ] ; then
        logprint "Resetting EEPROM to factory defaults..."
        /V2O/bin/clear_edid_list -all
    fi
}

# Cleanup audit log files
clean_audit_log_files() {
    /bin/rm -rf /data/reports/auditevent/*
}

# Reset all cofig to factory state
# If RESET_NETWORK==0, retain the network config
reset_config() {

    logprint "Resetting Configuration to factory defaults..."

    REMOTE_CONFIG=/etc/config/remote_config.xml
    CONFIG_FILE=/tmp/configfile
    FACTORYCONFIG_FILE=/etc/config/Device.xml
    devicename=`cat /etc/config/hw_profile*`
    /bin/rm -f $CONFIG_FILE
    /bin/touch $CONFIG_FILE
    if [ x"$RESET_NETWORK" = "x0" ] ; then

        logprint "Retaining network configuration."
        # First call the dumpConfig command that will write all the 
        # network config to /tmp/configfile
        /bin/cp ${REMOTE_CONFIG} ${REMOTE_CONFIG}.save
        sed -i -e 's/<Use>.*<\/Use>/<Use>File<\/Use>/' ${REMOTE_CONFIG}
        sed -i -e 's/<Mode>.*<\/Mode>/<Mode>append<\/Mode>/' ${REMOTE_CONFIG}
        /bin/rm ${CONFIG_FILE}
        /V2O/bin/dump_config --network
        /bin/mv ${REMOTE_CONFIG}.save ${REMOTE_CONFIG}

        # Make the /tmp/configfile look like one xml document
        # rooted at <CLIConfigs>
        /bin/echo "<CLIConfigs>" > ${CONFIG_FILE}.save
        /bin/grep -v '<?xml version="1.0"' ${CONFIG_FILE} >> ${CONFIG_FILE}.save
        /bin/echo "</CLIConfigs>" >> ${CONFIG_FILE}.save
        /bin/mv ${CONFIG_FILE}.save ${CONFIG_FILE}

		# Now call the java config converter code to create the Device.xml file
		# Call the xsl based apply only for the I50's
    	# Call the java entry point to create factory config
		AGENT=`get_device_interface_agent_name`
#		devicename=`cat /etc/config/hw_profile*`
		case "${AGENT}" in
			v2dtx|v2drx)
				/bin/cp /home/onair/objectXml/${devicename}.xml  ${FACTORYCONFIG_FILE}
				banner=`banner_get | urldecoder`
				xsltproc -o /tmp/stage1.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` --stringparam banner_str "${banner}" /etc/scripts/xsl/devicesaveconfig.xsl  /home/onair/objectXml/confighandler.xml
				xsltproc -o /tmp/stage2.xsl /tmp/stage1.xsl ${CONFIG_FILE}
				sed -i 's/No_BOOL_/false/g;s/Yes_BOOL_/true/g' /tmp/stage2.xsl
				xsltproc -o ${TMPCONFIG_FILE} /tmp/stage2.xsl ${FACTORYCONFIG_FILE}
				/bin/cp ${TMPCONFIG_FILE} ${FACTORYCONFIG_FILE}
			;;
		*)
			AGENT=`get_agent_app_name`
            if [ -f /etc/config/arm_system ] ; then 
               xsltproc -o /tmp/stage1.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/devicesaveconfig.xsl  /home/onair/objectXml/confighandler.xml
               xsltproc /tmp/stage1.xsl ${CONFIG_FILE} | sed -e 's/No_BOOL_/false/g' | sed -e 's/Yes_BOOL_/true/g' > /tmp/stage2.xsl

               xsltproc -o ${FACTORYCONFIG_FILE} /tmp/stage2.xsl /home/onair/objectXml/${devicename}.xml
            else
               java -cp /home/onair/modules/mediaroom/public/mediaroom.jar com.ipvs.device/${AGENT} FactoryConfig ${CONFIG_FILE} ${FACTORYCONFIG_FILE}
            fi
       esac
    else
        devicename=`cat /etc/config/hw_profile*`
        cp /home/onair/objectXml/${devicename}.xml  ${FACTORYCONFIG_FILE}
    fi

    FACTORY_CONFIG=/V2O/factory_config
    cd /tmp
    mkdir -p configs

    /bin/cp ${FACTORY_CONFIG}/* configs
    /bin/rm configs/*SKIP*.txt

    # Tar the config files
    tar -czf configs.tgz configs

    /bin/rm -rf configs
}


restart() {
    # Save to flash
    logprint "Saving configuration to flash..."
    /bin/cfmount

    # Add marker in compact flash to detect factory default at startup
    /bin/touch /mnt/cf/factory
 
    /bin/cp /etc/config/Device.xml /mnt/cf/Device.xml

    if [ -f /etc/config/arm_system ] ; then
        # Remove cache of CLI commands that fire on startup
        /bin/rm -f /mnt/cf/apply.sh

        # Remove factory configuration used to startup wizard
        /bin/touch /mnt/cf/factory
        /bin/rm -rf /mnt/cf/sys_type_xpi_*
        bin/cp /etc/config/Device.xml /mnt/cf/Device.xml
    fi

    /V2O/bin/sync_cp /tmp/configs.tgz /mnt/cf/configs.tgz
    /bin/rm -rf configs.tgz
   if [ -f /etc/config/arm_system ] ; then
     /bin/rm -rf /mnt/cf/cache
   fi
    /bin/cfunmount

    # And reboot
    logprint "Rebooting device."
    /bin/touch /tmp/reset


}

print_usage() {
    echo "Usage: $0 reset-network $1 delete-media"
}


if [ x"$1" = "xreset-network" ] ; then
    RESET_NETWORK=1
elif [ x"$1" = "xdelete-media" ] ; then
    DELETE_MEDIA=1
elif [ x"$1" != "x" ] ; then
    print_usage
    exit 1
fi

if  [ x"$2" = "xdelete-media" ] ; then            
    DELETE_MEDIA=1
elif [ x"$2" = "xreset-network" ] ; then
    RESET_NETWORK=1
elif [ x"$2" != "x" ] ; then
    print_usage
    exit 1
fi

stop_services
reset_openfire_defaults
reset_config
clean_media_files
clear_eeprom
clean_audit_log_files
restart

exit 0

esac
