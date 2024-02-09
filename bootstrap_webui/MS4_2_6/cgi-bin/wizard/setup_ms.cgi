#!/usr/bin/haserl -u 10000
Content-type: text/html
<?
source /usr/share/bweb/wizard/bweb_wizard.sh
?>
<?
echo ''
bweb_cli
/V2O/bin/save_config
?>
<?
	local status="OK"
#	If enablediscovery has a value (either 'on' or 'off') it means that
#	the wizard intentionally changed the value. So there is no need to check
#	if $FORM_cli_xmpp_server is '1' as that is implied. Otherwise, the
#	variable won't be set at all which is equivalent to an empty string. 

	if [ "${FORM_xmpp_server_checkbox_enablediscovery}" != "" ] || [ "${FORM_ntp_client_ipaddress}" != "" ] || [ "${FORM_ntp_server_checkbox_enable}" != "" ] || [ "${FORM_admin_password}" != "" ]; then
		status="REBOOT"
		echo "<div class='result'><status>${status}</status></div>"
		/V2O/bin/reboot_system
	else
		echo "<div class='result'><status>${status}</status></div>"
	fi
?>
