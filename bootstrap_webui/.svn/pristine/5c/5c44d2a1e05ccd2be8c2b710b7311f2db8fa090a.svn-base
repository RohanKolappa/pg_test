#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header 

?>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_cli
	fi
?>
<form class="form-panel" method="post" >
<h1 class="title">Management Server Statistics</h1>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_listdata get_xmpp_agent_status " Management Status"
		bweb_listdata get_ha_status " HA Status"
		bweb_listdata get_db_management_status " DB Management Status"
	fi
?>
<br>
<br>
<h1 class="title">Management Server Settings</h1>
<?		
	if [ $bweb_auth_level -ge 1 ]; then
		modelnumber=`cat /tmp/get_version.xml | grep 'name="hardware"' | cut -f4 -d'"'`
		bweb_getconfig ms_server_options
		bweb_admin_password 

		echo "<div class='table-header'>" 
		echo "<div class='table-header-icon'>" 
		echo "<a href='#' class='collapse-open'></a></div><h2>Administration Account</h2></div>"
		echo "<div class='sectiondiv' style='display:none;'>" 
		echo '<table class="formTable" width="100%" >'
		echo '<tbody>'
		echo '<tr>'
		echo '<td class="table-label">Admin Password</td>'
		echo '<td class="table-text">'
		echo "<input type=\"password\" id=\"adminpwd\" name=\"adminpwd\" value=\"Oo0Oo0Oo0\" class=\"validate[required,custom[password]]\">"
		echo '</td>'
		echo '<td class="table-label">'
		echo '</td>'
		
		echo '<td class="table-label">Retype Password</td>'
		echo '<td class="table-text">'
		bweb_upgrade_url
		echo "<input type=\"password\" id=\"retypeadminpwd\" name=\"retypeadminpwd\" value=\"Oo0Oo0Oo0\" class=\"validate[required,equals[adminpwd]]\">"
		echo '</td>'
		echo '<td class="table-label">'
		echo '</td>'
		
		echo '</tr>'
		echo '</tbody>'
		echo '</table><br><p>*After making changes to admin password,<ol>'
		echo '<li>Automatic save settings will take place and any unsaved config changes done since last save will be saved.</li>'
		echo '<li>Log into installed admin UI with new password, select this device, right click, edit device and enter this new password.</li>'
		echo '<li>Re-login into this UI with the new password to do more changes.</li></ol></p></div>'

		echo "<div class='table-header'>" 
		echo "<div class='table-header-icon'>" 
		echo "<a href='#' class='collapse-open'></a></div><h2>System Control</h2></div>"
		echo "<div class='sectiondiv' style='display:none;'>" 
		echo '<table class="formTable" width="100%" >'
		echo '<tbody>'
		echo '<tr>'
		echo '<td class="table-text" colspan="2" width="50%"><input type="button" id="revert" class="cmdbutton" value="Factory Reset"></td>'
		echo '<td class="table-text" colspan="2" width="50%">'
		echo '<input type="button" id="reboot" class="cmdbutton" value="Reboot" >'
		echo '<input type="button" id="shutdown" class="cmdbutton" value="Shutdown">'
		echo '</td>'
		echo '</tr>'
		echo '<tr>'
		echo '<td class="table-label" width="22%">Reset network settings</td>'
		echo '<td class="table-text" width="28%"><input type="checkbox" id="reset_network"></td>'
		echo '</tr>'
		echo '</tbody>'
		echo '</table></div>'
		echo "<div class='table-header'>" 
		echo "<div class='table-header-icon'>" 
		echo "<a href='#' class='collapse-open'></a></div><h2>Upgrade</h2></div>"
		echo "<div class='sectiondiv' style='display:none;'>" 
		echo '<table class="formTable" width="100%" >'
		echo '<tbody>'
		echo '<tr>'
		echo '<td class="table-label">URL </td>'
		echo '<td class="table-text">'
		bweb_upgrade_url
		echo "<input type=\"text\" id=\"upgrade_url\" value=\"$upgrade_url\">"
		echo '<input type="button" id="upgrade" class="cmdbutton" value="Upgrade" >'
		echo '</td>'
		echo '<td class="table-label">'
		echo '</td>'
		echo '</tr>'
		echo '</tbody>'
		echo '</table></div>'
if [ "$modelnumber" = "MS-150" ]; then
		echo "<div class='table-header'>" 
		echo "<div class='table-header-icon'>" 
		echo "<a href='#' class='collapse-open'></a></div><h2>License Setup</h2></div>"
		echo "<div class='sectiondiv' style='display:none;'>" 
		echo '<table class="formTable" width="100%" >'
		echo '<tbody>'
		echo '<tr>'
		echo '<td class="table-text">'
		echo '<iframe src="/cgi-bin/license-upload.cgi" height="200" width="550">'
		echo 'License upload requires a web browser that supports iFrames.'
		echo '</iframe>'
		echo '</td>'
		echo '</tr>'
		echo '</tbody>'
		echo '</table></div>'
fi
		bweb_getconfig ha_configuration
		bweb_getconfig ldap_configuration
		bweb_getconfig db_management
		bweb_getconfig demos_mode
		bweb_addbuttons 
	fi
?>
</form>


<? bweb_footer ?>
