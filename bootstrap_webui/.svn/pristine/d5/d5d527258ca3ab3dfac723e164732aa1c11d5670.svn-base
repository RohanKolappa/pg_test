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
			bweb_addbuttons 
		fi
	?>
	</form>


<? bweb_footer ?>