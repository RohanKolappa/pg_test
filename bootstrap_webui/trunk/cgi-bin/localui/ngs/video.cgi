#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header 


?>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_cli
			if [ x"${POST_cli_edid_list}" = "x1" ]; then
			   /V2O/bin/load_edid.sh > /dev/NULL 2>&1 
			fi
		fi
	?>
	<form class="form-panel" method="post" >
	<h1 class="title">Video Diagnostics</h1>
	<? 
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_listdata get_eeprom_edid "EDID to Video Source"
			bweb_listdata get_monitor_edid "Monitor EDID"
		fi
	?>
	<h1 class="title">Video Settings</h1>
	<?                              
		bweb_getboardtype         
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_getconfig sync_edid_eeprom_parm
			bweb_getconfig edid_videoinput
			bweb_getlistconfig lut_list 
			syncedideeprom=`grep enable /tmp/get_sync_edid_eeprom_parm.xml| cut -f4 -d'"'`
			if [ x"$syncedideeprom" = "xNo" ]; then
				bweb_getlistconfig edid_list 
			fi
		fi
		if [ x"$boardtype" = "xENC" ]; then
			if [ $bweb_auth_level -ge 1 ]; then
				bweb_getconfig xp_tx_video_parm
			fi
		fi
		if [ x"$boardtype" = "xDEC" ]; then
			if [ $bweb_auth_level -ge 1 ]; then
				bweb_getconfig rx_input_video_parm
			fi
		fi
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_addbuttons 
		fi
	?>
	</form>

<? bweb_footer ?>
