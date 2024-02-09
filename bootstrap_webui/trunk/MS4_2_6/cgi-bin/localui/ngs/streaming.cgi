#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header 


?>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_cli
		if [ x"$POST_cli_v2d_streaming_profile" = "x1" ] ; then
			/V2O/bin/update_conn_parm.sh > /dev/NULL 2>&1 
		fi
	fi
?>
<form class="form-panel" method="post">
<h1 class="title">Streaming Statistics</h1>
<?                              
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_getboardtype         
		if [ x"$boardtype" = "xDEC" ]; then
			bweb_listdata get_rx_connection_status "Connection Information"
		else
			bweb_listdata get_tx_connection_status "Connection Information"
		fi
	fi
?>
<h1 class="title">Streaming Settings</h1>
<?                              
	if [ $bweb_auth_level -ge 1 ]; then
		bweb_getboardtype         
		if [ x"$boardtype" = "xDEC" ]; then
#			bweb_getconfig v2d_connect "ajax_submit:Connect" 
			bweb_getconfig rx_splash_parm
			bweb_getconfig reverse_audio 
			bweb_getlistconfig conn_table 
		else
			bweb_getconfig v2d_streaming_profile 
			bweb_getconfig tx_splash_parm
			bweb_getconfig splash_mode "ajax_submit:Splash Mode" 
			bweb_getconfig tx_control_ports_parm 
			bweb_getconfig multicast_parm 
			bweb_getconfig reverse_audio 
			showauthtable=`grep enable_auth_table /tmp/get_v2d_streaming_profile.xml| cut -f4 -d'"'`
			if [ x"${showauthtable}" = "xYes" ]; then
				bweb_getlistconfig auth_table 
			fi
		fi
		bweb_addbuttons 
	fi
?>
</form>

<? bweb_footer ?>
