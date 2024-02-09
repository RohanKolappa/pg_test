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
	fi
?>
<br>
<br>
<h1 class="title">Management Server Settings</h1>
<?		
	if [ $bweb_auth_level -ge 1 ]; then

		bweb_getconfig dms_server_options
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
		echo "<input type=\"password\" id=\"adminpwd\" name=\"adminpwd\" value=\"$adminpwd\" class=\"validate[required,custom[password]]\">"
		echo '</td>'
		echo '<td class="table-label">'
		echo '</td>'
		
		echo '<td class="table-label">Retype Password</td>'
		echo '<td class="table-text">'
		bweb_upgrade_url
		echo "<input type=\"password\" id=\"retypeadminpwd\" name=\"retypeadminpwd\" value=\"$adminpwd\" class=\"validate[required,equals[adminpwd]]\">"
		echo '</td>'
		echo '<td class="table-label">'
		echo '</td>'
		
		echo '</tr>'
		echo '</tbody>'
		echo '</table></div>'

		echo "<div class='table-header'>" 
		echo "<div class='table-header-icon'>" 
		echo "<a href='#' class='collapse-open'></a></div><h2>System Control</h2></div>"
		echo "<div class='sectiondiv' style='display:none;'>" 
		echo '<table class="formTable" width="100%" >'
		echo '<tbody>'
		echo '<tr>'
		echo '<td>'
		echo '<input type="button" id="revert" class="cmdbutton" value="Factory Reset">'
		echo '<input type="button" id="reboot" class="cmdbutton" value="Reboot" >'
		echo '<input type="button" id="shutdown" class="cmdbutton" value="Shutdown">'
		echo '</td>'
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
		
		bweb_setDeviceType
		if [ "$is_combo" == "Yes" ]; then
			bweb_getconfig ldap_configuration
			bweb_getconfig db_management
		fi
		
		bweb_addbuttons 
	fi
?>
</form>


<? bweb_footer ?>
