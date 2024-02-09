#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Applying configuration..."

# Fix for Bug 25986
if [ -f /etc/config/sw_profile_v2d.txt ] ; then
    /V2O/bin/loadedideeprom
fi

# Load drivers on XPi
if [ -f /etc/config/sw_profile_xpi.txt ] ; then
   # load I2C switch driver
   logprint "Loading driver for I2C switch"
   /V2O/bin/setgpio.sh 97 out 0
   /V2O/bin/setgpio.sh 97 out 1

   # create watchdog device and load watchdog driver
   logprint "Creating watchdog device"
   mknod /dev/watchdog c 10 130
   logprint "Loading watchdog driver"
   insmod /V2O/modules/pxa168_wdt.ko

   # configure network drivers
   insmod /V2O/modules/pca954x.ko
   logprint "Starting test.sh"
   /V2O/bin/test.sh
   logprint "Finished test.sh"

   # configure audio driver
   logprint "Setting up audio ..."
   /V2O/bin/maint setup_audio
   logprint "Finished setup_audio"
fi

# Check if we need to start in console recovery mode
USERIN=0
if [ -f /etc/config/sw_profile_v2dxp200.txt ] ; then
    echo "Press ESC to boot in Console Recovery Mode ..."
    USERIN=$(/V2O/bin/getuserinput)
    if [ "$USERIN" = "1" ] ; then
        /V2O/bin/set_kbconsole -kbconsole Yes > /dev/null 2>&1
    fi
fi


if [ ! -f /usr/bin/java ] ; then
    #
    # TODO: workaround for right now till java starts working on XPi
    #
    #    rm -f /etc/config/hw_profile*.txt
    #    rm -f /etc/config/sw_profile*.txt
    rm -rf /var/local
    mkdir /var/local
    cp /home/onair/templates/interfaces /var/local
else
    #
    # PSCW: don't clean out default config files on XPi till java works
    #

    /bin/rm -f /V2O/config/routeconf.txt
    touch /V2O/config/routeconf.txt

    /bin/rm -f /V2O/config/lutlistconf_0.txt
    touch /V2O/config/lutlistconf_0.txt

    /bin/rm -f /V2O/config/lutlistconf_1.txt
    touch /V2O/config/lutlistconf_1.txt

    /bin/rm -f /V2O/config/luttable.txt
    touch /V2O/config/luttable.txt

    /bin/rm -f /V2O/config/iflistconf.txt
    touch /V2O/config/iflistconf.txt

    /bin/rm -f /V2O/config/nasserverlistconf.txt
    touch /V2O/config/nasserverlistconf.txt

    /bin/rm -f /V2O/config/natzonelistconf.txt
    touch /V2O/config/natzonelistconf.txt

    /bin/rm -f /V2O/config/hostlistconf.txt
    touch /V2O/config/hostlistconf.txt

   #
   # Call the Java apply handler that will invoke CLI commands
   # for all configuration found in  Device.xml
   #
AGENT=`get_device_interface_agent_name`
	case "${AGENT}" in
		v2dtx|v2drx)
			xsltproc -o /tmp/apply.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/deviceconfig.xsl  /home/onair/objectXml/confighandler.xml
			xsltproc /tmp/apply.xsl /etc/config/Device.xml | sed -e 's/false/No/g' | sed -e 's/true/Yes/g' > /tmp/apply.sh
			/bin/sh -x /tmp/apply.sh > /var/log/deviceapp/apply_config.log 2>&1
			;;
		*)
			AGENT=`get_agent_app_name`
            if [ ! -f /etc/config/arm_system ] ; then
                /usr/bin/java -cp /home/onair/modules/mediaroom/public/mediaroom.jar com.ipvs.device.${AGENT} ApplyConfig /etc/config/Device.xml > /var/log/deviceapp/apply_config.log 2>&1
            else
                if [ ! -f /mnt/cf/apply.sh ]; then
                    xsltproc -o /tmp/apply.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/deviceconfig.xsl  /home/onair/objectXml/confighandler.xml
				   xsltproc /tmp/apply.xsl /etc/config/Device.xml | sed -e 's/false/No/g' | sed -e 's/true/Yes/g' > /tmp/apply.sh
                     cp /tmp/apply.sh /mnt/cf/apply.sh
                     /bin/cfunmount
                else
                     cp /mnt/cf/apply.sh /tmp/apply.sh
                fi
                 /bin/sh -x /tmp/apply.sh > /var/log/deviceapp/apply_config.log 2>&1

            fi
	esac

fi


if [ -f /etc/config/sw_profile_xpi.txt ] ; then
    #Begin audit of configuration to the log file
    /bin/cp ${REMOTE_CONFIG} ${REMOTE_CONFIG}.save
    sed -i -e 's/<Use>.*<\/Use>/<Use>File<\/Use>/' ${REMOTE_CONFIG}
    sed -i -e 's/<Mode>.*<\/Mode>/<Mode>append<\/Mode>/' ${REMOTE_CONFIG}
    /bin/rm -f ${CONFIG_FILE}


    logprint "Creating a cache of configuration"
    for i in admin adminop auth_table color_parm conn_table dns_client if_list lut_list multicast_parm ntp reverse_audio route_table rx_input_video_parm rx_splash_parm sync_edid_eeprom_parm tx_connection_parm tx_control_ports_parm tx_rfb_parm tx_splash_parm tx_video_parm xmpp_agent xp_audio_mixer display_parm
    do
	/V2O/bin/describe_$i > /tmp/describe_$i.xml
	/V2O/bin/get_$i > /tmp/get_$i.xml
done
    # Special handling for some parameters done by the WebUI
    # Do not cache the describe commands for these
    for i in auth_table if_list tx_connection_parm display_parm
    do
    	/bin/rm -f /tmp/describe_$i.xml
    done
fi

# Directly apply the configs using V2D CLI commands for
# things not yet handled by Java apply handler above
if [ -f /etc/config/sw_profile_xpi.txt ] ; then
   logprint "Configuring license in driver ..."
   /V2O/bin/maint setlicense
fi

if [ -f /etc/config/sw_profile_xpi.txt ] ; then
   logprint "Configuring default EDID ..."
   VIDINPUT=`/V2O/bin/get_edid_videoinput |grep videoinput |sed 's/.*value="\(.*\)".*/\1/'`
   logprint "Switching video input to ${VIDINPUT} ..."
   if [ x${VIDINPUT} != x"Analog" ] ; then
    /V2O/bin/loadedideeprom -c 1
   else
    /V2O/bin/loadedideeprom -c 1 -A 0
   fi
   AUDINPUT=`/V2O/bin/get_xp_audio_mixer |grep audio_input |sed 's/.*value="\(.*\)".*/\1/'`
   logprint "Switching audio input to ${AUDINPUT} ..."
   /V2O/bin/set_xp_audio_mixer -audio_input ${AUDINPUT} 2>&1 >> /dev/NULL

   logprint "Enabling EDID access and video input from PC ..."
   if [ -f /sys/class/gpio/gpio98 ]; then
       echo 98 > /sys/class/gpio/unexport
   fi
   echo 98 > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio98/direction
   echo 1 > /sys/class/gpio/gpio98/value
   echo 98 > /sys/class/gpio/unexport

   if [ -f /sys/class/gpio/gpio99 ]; then
       echo 99 > /sys/class/gpio/unexport
   fi
   echo 99 > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio99/direction
   echo 1 > /sys/class/gpio/gpio99/value
   echo 99 > /sys/class/gpio/unexport

   logprint "Setting decoder Splash Screen to Grid Lines..."
   /V2O/bin/set_rx_splash_parm -splash_screen "Grid Lines" >> /dev/NULL
fi

#/V2O/bin/loadnet
#/V2O/bin/load_eth2_if
#/V2O/bin/loadnet -r
#/V2O/bin/load_ntp
#/V2O/bin/load_ntp_server
#/V2O/bin/load_dhcp_server
#/V2O/bin/load_xmpp_agent
#/V2O/bin/load_admin
#/V2O/bin/load_ha

/V2O/bin/load_tx_kbm_layout
/V2O/bin/load_rx_kbm_layout

/V2O/bin/load_demos_mode

# Start the Busybox watchdog
if [  -f /etc/config/arm_system ] ; then
    /sbin/watchdog -t 10 /dev/watchdog
fi

# Start the wathdog script
/etc/scripts/watchdog.sh &

logprint "Done Applying configuration."

# Display message if we've started in console recovery mode
if [ "$USERIN" = "1" ] ; then
	echo "!! CONSOLE RECOVERY MODE !!"
	echo "Please reset Keyboard Bootup Mode."
fi


