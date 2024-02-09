#!/usr/bin/haserl -u 10000
Content-type: text/html
<?
if [ ! -f /tmp/get_if_list.xml ]; then
  /V2O/bin/get_if_list > /tmp/get_if_list.xml
fi
export old_inet=`ifconfig eth0 | head -2 | tail -1 | cut -f2 -d':' | cut -f1 -d' '`
export old_nmsk=`ifconfig eth0 | head -2 | tail -1 | cut -f4 -d':' | cut -f1 -d' '`
export old_dhcp=`grep 'name="usedhcp"' /tmp/get_if_list.xml | cut -f4 -d'"' | head -1`
source /usr/share/bweb/wizard/bweb_wizard.sh
echo ''
bweb_cli
errcnt=0
if [ -f /tmp/cmd.results ]; then
	errcnt=`grep -c 'name="Status" value="Failure"' /tmp/cmd.results`
fi
if [ $errcnt -gt 0 ]; then
	echo Errors detected: ${errcnt}. Reverting back to old network settings. >> /tmp/cmd.log
	echo /V2O/bin/mod_if_list -usedhcp $old_dhcp -ipaddress $old_inet -netmask $old_nmsk 1 >> /tmp/cmd.log
	/V2O/bin/mod_if_list -usedhcp $old_dhcp -ipaddress $old_inet -netmask $old_nmsk 1
	/V2O/bin/get_if_list >> /tmp/get_if_list.xml
	echo "<div class='result'><status>ERROR</status></div>"
else
	/V2O/bin/save_config
	local status="OK"
	if [ x"${FORM_board_info_boardtype}" = "xStreamDst" ]; then
		status="REBOOT"
		echo "<div class='result'><status>${status}</status></div>"
		/V2O/bin/reboot_system
	else
		echo "<div class='result'><status>${status}</status></div>"
	fi
fi
?>
