#!/bin/ash
# barco web interface functions
# (C) 2009-2011 Peter Korsgaard <peter.korsgaard@barco.com>

bweb_setup() {
	local hostname=`uname -n`
	local serial=`cat /tmp/get_version.xml | grep 'name="serialnumber"' | cut -f4 -d'"'`
	local modelnumber=`cat /tmp/get_version.xml | grep 'name="hardware"' | cut -f4 -d'"'`
	local scenable=`/V2O/bin/get_xmpp_server | grep 'name="enable"' | cut -f4 -d'"'`
	local scenableno=''
	local scenableyes=''
	local devicedesc=''
	local softwareVersion=`/V2O/bin/get_version | grep 'name="swvers"' | cut -f4 -d'"'`
	local health=`/V2O/bin/get_xmpp_agent_status | grep 'name="Status"' | awk -F"[<>]" '/Parameter/ {getline; print $3;exit}'`
	local autodetection=`/V2O/bin/get_xmpp_agent | grep 'name="enable"' | cut -f4 -d'"'`
	local isDmscombo=`/V2O/bin/get_xmpp_server | grep 'name="enable"' | cut -f4 -d'"'`
	
	if [ "$scenable" = "Yes" ]; then
		devicedesc='Management Server \/ Digital Media Server'
		scenableno=''
		scenableyes='selected'
	else
		devicedesc='Media Server'
		scenableno='selected'
		scenableyes=''
	fi
	local usedhcp=`/V2O/bin/get_if_list | grep 'name="usedhcp"' | head -1 | cut -f4 -d'"'`
	local ipaddress=`/V2O/bin/get_if_list | grep 'name="ipaddress"' | head -1 | cut -f4 -d'"'`
	local netmask=`/V2O/bin/get_if_list | grep 'name="netmask"' | head -1 | cut -f4 -d'"'`
	local gateway=`/V2O/bin/get_route_table | grep 'name="gw"' | head -1 | cut -f4 -d'"'`
	local scname=`/V2O/bin/get_xmpp_agent | grep 'name="devicename"' | cut -f4 -d'"'`
	local scpw=`/V2O/bin/get_xmpp_agent | grep 'name="password"' | cut -f4 -d'"'`
	local scip=`/V2O/bin/get_xmpp_agent_status | grep 'name="XMPP Server"' | awk -F"[<>]" '/Parameter/ {getline; print $3;exit}'`
	local enablediscovery=`/V2O/bin/get_xmpp_server | grep 'name="enablediscovery"' | cut -f4 -d'"'`
	local multipleaccounts=`/V2O/bin/get_xmpp_server | grep 'name="enableuserregistry"' | cut -f4 -d'"'`
	local ntpclientipaddress=`/V2O/bin/get_ntp | grep 'name="ipaddress"' | cut -f4 -d'"'`
	local ntpserverenabled=`/V2O/bin/get_ntp_server | grep 'name="enable"' | head -1 | cut -f4 -d'"'`
	local adminpwd=`/V2O/bin/get_admin | grep 'name="password"' | head -1 | cut -f4 -d'"'`
	
	local dhcpchecked=''
	if [ "$usedhcp" = "Yes" ]; then
		dhcpchecked=checked
	fi
	local discoverychecked=''
	if [ "$enablediscovery" = "Yes" ]; then
		discoverychecked=checked
	fi
	local multiaccountschecked=''
	if [ "$multipleaccounts" = "Yes" ]; then
		multiaccountschecked=checked
	fi
	
	local autodetected=''
	if [ "$autodetection" = "No" ]; then
		autodetected=checked
	fi
	
	local ntpserverchecked=''
	if [ "$ntpserverenabled" = "Yes" ]; then
		ntpserverchecked=checked
	fi
	
	local isdmscombochecked=''
	if [ "$isDmscombo" = "Yes" ]; then
		isdmscombochecked=checked
	fi
	
	local ispuredmschecked=''
	if [ "$isDmscombo" = "No" ]; then
		ispuredmschecked=checked
	fi

	local isFactoryDefault=''
	if [ -f /etc/config/factory ]; then
		isFactoryDefault="true"
	else
		isFactoryDefault="false"
	fi
	
	bweb_locate_res '/home/onair/htdocs/public/templates/wizard/' 'setup' '.html'
	cp $bweb_resource /tmp/setup.html
	sed -ie s/HOSTNAME/${hostname}/ /tmp/setup.html
	sed -ie s/SCENABLEYES/${scenableyes}/ /tmp/setup.html
	sed -ie s/SCENABLENO/${scenableno}/ /tmp/setup.html
	sed -ie s/MODELNUMBER/${modelnumber}/ /tmp/setup.html
	sed -ie "s/DEVICEDESC/${devicedesc}/" /tmp/setup.html
	sed -ie s/SERIAL/${serial}/ /tmp/setup.html
	sed -ie s/DHCPCHECKED/${dhcpchecked}/ /tmp/setup.html
	sed -ie s/IPADDRESS/${ipaddress}/ /tmp/setup.html
	sed -ie s/NETMASK/${netmask}/ /tmp/setup.html
	sed -ie s/GATEWAY/${gateway}/ /tmp/setup.html
	sed -ie s/MSMANUAL/${msmanual}/ /tmp/setup.html
	sed -ie s/SCNAME/${scname}/ /tmp/setup.html
	sed -ie s/SCPW/${scpw}/ /tmp/setup.html
	sed -ie s/SCIP/${scip}/ /tmp/setup.html
	sed -ie s/DISCOVERYCHECKED/${discoverychecked}/ /tmp/setup.html
	sed -ie s/MULTIACCOUNTSCHECKED/${multiaccountschecked}/ /tmp/setup.html
	sed -ie s/SOFTWAREVERSION/${softwareVersion}/ /tmp/setup.html
	sed -ie s/HEALTH/${health}/ /tmp/setup.html
	sed -ie s/NTPCLIENTIP/${ntpclientipaddress}/ /tmp/setup.html
	sed -ie s/NTPSERVERCHECKED/${ntpserverchecked}/ /tmp/setup.html
	sed -ie s/AUTODETECT/${autodetected}/ /tmp/setup.html
	sed -ie s/ISCOMBO/${isdmscombochecked}/ /tmp/setup.html
	sed -ie s/ISPUREDMS/${ispuredmschecked}/ /tmp/setup.html
	sed -ie s/ISFACTORYDEFAULT/${isFactoryDefault}/ /tmp/setup.html
	sed -ie s/ADMINPASSWORD/${adminpwd}/ /tmp/setup.html

	cat /tmp/setup.html
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_setup" >&2
}

