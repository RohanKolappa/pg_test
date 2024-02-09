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
	<h1 class="title">Storage Statistics</h1>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_listdata get_disk_status "Disk Status "
			bweb_liststatus get_nas_status "NAS Status"
		fi
	?>
	<br>
	<br>
	<h1 class="title">Storage Settings</h1>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then 
			bweb_getlistconfig nas_list
			echo "<div class='table-header'>" 
			echo "<div class='table-header-icon'>" 
			echo "<a href='#' class='collapse-open'></a></div><h2>Rebuild Disk</h2></div>"
			echo "<div class='sectiondiv' style='display:none;'>" 
			echo '<table class="formTable" width="100%" >'
			echo '<tbody>'
			echo '<tr>'
			echo '<td class="table-label">Disk ID </td>'
			echo '<td class="table-text">'
			#bweb_upgrade_url
			echo "<input type=\"text\" id=\"disk_id\" value=\"\" class=\"validate[custom[integer]]\">"
			echo '<input type="button" id="rebuildDisk" class="cmdbutton" value="rebuild" >'
			echo '</td>'
			echo '<td class="table-label">'
			echo '</td>'
			echo '</tr>'
			echo '</tbody>'
			echo '</table></div>'
			bweb_addbuttons 
		fi
	?>
	</form>


<? bweb_footer ?>