#!/bin/sh
#
# prepare hardware of ethernet interface

# Source common library
. /etc/scripts/common.rc

setMacAddress()
{
	MACADDRESS=`fw_printenv ethaddr | cut -d '=' -f2`

	# Check for mac address formatted with colons: remove the colons
	MACADDRESS=`echo $MACADDRESS | sed -e 's/://g'`

	if [ $MACADDRESS != "0425feed0018" ] ;  then
	    if [ -d /sys/class/omnitek/OmniTekBAR0/device/net ] ;  then
		  INTERFACE=`ls /sys/class/omnitek/OmniTekBAR0/device/net/`

		  echo Setting mac for $INTERFACE to $MACADDRESS
		  ifconfig $INTERFACE hw ether `echo $MACADDRESS`
	    fi
   else
		logprint "Unit has default mac address -> switching off auto ethernet configuration."
		rm /var/local/interfaces
   fi	

   if [ ! -d /sys/class/omnitek/OmniTekBAR0/device/net ] ;  then
     if [ ! -f /mnt/cf/eth_notup ] ; then
	   touch /mnt/cf/eth_notup
       /V2O/bin/reboot_system
	 fi
   else
     if [ -f /mnt/cf/eth_notup ] ; then
	  rm -rf /mnt/cf/eth_notup
      logprint "Network / Ethernet failed on last try"
      fi
   fi

}

case "$1" in
	start|"")
		echo "setting MAC address"
		setMacAddress
		;;
	stop) ;;
	*)
		echo "Usage: $0 {start|stop}" >&2
		exit 1
		;;
esac
