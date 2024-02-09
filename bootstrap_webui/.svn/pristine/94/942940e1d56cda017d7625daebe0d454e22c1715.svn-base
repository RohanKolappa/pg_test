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
<h1 class="title">Administration</h1>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_listdata get_xmpp_agent_status " Management Status"
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
		echo '<input type="button" id="toggleboardtype" class="cmdbutton" value="Encoder/Decoder">'
		echo '</td>'
		echo '</tr>'
		echo '<tr hidden>'
		echo '<td class="table-label" width="22%">Reset network settings</td>'
		echo '<td class="table-text" width="28%"><input type="checkbox" id="reset_network" checked></td>'
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
		echo '<h1 class="title">Management Settings</h1>'
		bweb_getconfig xmpp_agent 
		bweb_getconfig admin 
		bweb_getconfig adminop 
		bweb_addbuttons 
	fi
?>
</form>


<? bweb_footer ?>
