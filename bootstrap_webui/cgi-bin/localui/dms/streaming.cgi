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
	<? 
		if [ $bweb_auth_level -ge 1 ]; then 
			bweb_getconfig tx_control_ports_parm 
			bweb_getconfig rx_av_ports_parm
			bweb_getconfig multicast_parm
			bweb_addbuttons 
		fi
	?>
	</form>


<? bweb_footer ?>

