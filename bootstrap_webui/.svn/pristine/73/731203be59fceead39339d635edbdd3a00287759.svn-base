#!/bin/ash
# barco web interface functions
# (C) 2009-2011 Peter Korsgaard <peter.korsgaard@barco.com>

bweb_setup() {
echo "I AM HERE IN NGS SETUP " >> /tmp/megm.log
	hostname=`uname -n`
	serial=`fw_printenv AssemblySerialNumber | cut -f2 -d'='`
	if [ -f /var/status/dhcp_eth0.status ]; then
		gateway=`grep '^router=' /var/status/dhcp_eth0.status | cut -f2 -d'='`
		ipaddress=`grep '^ip=' /var/status/dhcp_eth0.status | cut -f2 -d'='`
		netmask=`grep '^subnet=' /var/status/dhcp_eth0.status | cut -f2 -d'='`
	else
		gateway=""
		ipaddress=""
		subnet=""
	fi
    local xmpp_server=`/V2O/bin/get_xmpp_agent | grep 'name="serverip"' | sed -e 's/^.*value="\(.*\)".*$/\1/'`
	local standalone=`/V2O/bin/get_adminop | grep 'name="standalone"' | sed -e 's/^.*value="\(.*\)".*$/\1/'`
	local multicast=`/V2O/bin/get_multicast_parm | grep 'name="multicast_from"' | sed -e 's/^.*value="\(.*\)".*$/\1/'`
	local available_locales=`/V2O/bin/describe_tx_kbm_layout | grep 'name="keyboard_layout"' | sed -e 's/^.*values="\(.*\)".*$/\1/'`
	local default_locale=`/V2O/bin/get_tx_kbm_layout | grep 'name="keyboard_layout"' | sed -e 's/^.*value="\(.*\)".*$/\1/'`
	
	bweb_locate_res '/var/www/templates/wizard/' 'setup' '.html'
	echo "============= BWEB_NGS =============== $bweb_resource " >> /tmp/megm.log
	cp $bweb_resource /tmp/setup.html
	
	sed -ie s/HOSTNAME/${hostname}/ /tmp/setup.html
	sed -ie s/SERIAL/${serial}/ /tmp/setup.html
	sed -ie s/MULTICAST_FROM/${multicast}/ /tmp/setup.html
	sed -ie "s/KBM_DEFAULT_VALUE/${default_locale}/" /tmp/setup.html
	
	# Added double quotes as the available_locales string contains special characters like spaces, commas and curly braces.
	sed -ie "s/ALL_KBM_LOCALES/${available_locales}/" /tmp/setup.html
	
	if [ "$standalone" = "Yes" ]; then
	  sed -ie 's/STANDALONE/checked/' /tmp/setup.html
	  sed -ie 's/MANAGEDMD//' /tmp/setup.html
	  sed -ie 's/XMPPDISABLED/disabled/' /tmp/setup.html
	else
	  sed -ie 's/STANDALONE//' /tmp/setup.html
	  sed -ie 's/MANAGEDMD/checked/' /tmp/setup.html
	  sed -ie 's/XMPPDISABLED//' /tmp/setup.html
	fi
	sed -ie s/XMPP_SERVER/${xmpp_server}/ /tmp/setup.html
	#sed -ie s/IPADDRESS/${ipaddress}/ /tmp/setup.html
	#sed -ie s/NETMASK/${netmask}/ /tmp/setup.html
	#sed -ie s/GATEWAY/${gateway}/ /tmp/setup.html
	cat /tmp/setup.html
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_setup" >&2
}

