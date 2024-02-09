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
	<h1 class="title">Network Statistics</h1>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_listdata get_network_ports_status "Interface "
			bweb_liststatus get_route_status "Routing"
		fi
	?>
	<br>
	<br>
	<h1 class="title">Network Settings</h1>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then 
			bweb_getlistconfig if_list
			bweb_getconfig dns_client 
			bweb_getlistconfig route_table
			bweb_getlistconfig host_list
			bweb_addbuttons 
		fi
	?>
	</form>


<? bweb_footer ?>
