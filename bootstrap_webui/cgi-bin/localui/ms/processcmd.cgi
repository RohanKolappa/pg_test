#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/localui/bweb_localui.sh
#bweb_header 1
bweb_header
cmd_error_begin() {
	echo -n '<results>'	
}
cmd_error_end() {
	echo '</results>'
}
?>
<? 
	if [ $bweb_auth_level -ge 1 ]; then
		case ${GET_id} in
			revert)
				echo "Executing ..." /V2O/bin/revert_config > /tmp/cmd.log
				status="REVERT"
				/V2O/bin/revert_config -reset_network ${GET_reset_network} 2>&1 >> /tmp/cmd.log
				;;
			shutdown)
				echo "Executing ..." /V2O/bin/shutdown_system > /tmp/cmd.log
				status="SHUTDOWN"
				/V2O/bin/shutdown_system 2>&1 >> /tmp/cmd.log
				;;
			reboot)
				echo "Executing ..." /V2O/bin/reboot_system > /tmp/cmd.log
				status="REBOOT"
				/V2O/bin/reboot_system 2>&1 >> /tmp/cmd.log
				;;
			save_settings)
				echo "Executing ..." /V2O/bin/save_config > /tmp/cmd.log
				/V2O/bin/save_config 2>&1 >> /tmp/cmd.log
				status="OK"
				;;
			toggleboardtype)
				echo "Executing ..." /V2O/bin/set_board_info > /tmp/cmd.log
				if [ -f /etc/config/sys_type_xpi_encoder ]; then
					newtype="StreamDst"
				else
					newtype="StreamSrc"
				fi
				/V2O/bin/set_board_info -boardtype ${newtype} 2>&1 > /tmp/cmd.log 
				/V2O/bin/save_config 2>&1 >> /tmp/cmd.log
				/V2O/bin/reboot_system 2>&1 >> /tmp/cmd.log
				status="REBOOT"
				;;
			upgrade)
				echo "Executing ..." /V2O/bin/upgrade_device > /tmp/cmd.log
				status="REBOOT"
				/V2O/bin/upgrade_device -url ${FORM_url} 2>&1 >> /tmp/cmd.log
				local errmsg=`grep -c 'ERROR:' /tmp/cmd.log`
				if ( [ $errmsg != 0 ] ); then
					cmd_error_begin
						tail -3 /tmp/cmd.log | tr '\n' ' '
					cmd_error_end
					status="ERROR"
				fi
				;;
			make_conn)
				echo "Executing ... /V2O/bin/make_conn -channelNumber 1 ${GET_idx}" > /tmp/cmd.log
				status="OK"
				/V2O/bin/make_conn -channelNumber 1 ${GET_idx} 2>&1 >> /tmp/cmd.log
				;;
			break_conn)
				echo "Executing ... /V2O/bin/break_conn -channelNumber 1" > /tmp/cmd.log
				status="OK"
				/V2O/bin/break_conn -channelNumber 1 2>&1 >> /tmp/cmd.log
				;;
			clearedid)
				echo "Executing ... /V2O/bin/clear_edid_list -channelNumber 1" > /tmp/cmd.log
				status="REFRESH"
				/V2O/bin/clear_edid_list -channelNumber 1 2>&1 >> /tmp/cmd.log
				;;
			updateedid)
				echo "Executing ... /V2O/bin/loadedideeprom -c 1" > /tmp/cmd.log
				status="OK"
				/V2O/bin/load_edid.sh  2>&1 >> /tmp/cmd.log
				;;
			ajax_submit)
				bweb_cli
				status="OK"
				;;
			*)
				echo "Unknown Command " ${GET_id} > /tmp/cmd.log
				cmd_error_begin
					echo "Unknown Command " ${GET_id}
				cmd_error_end
				status="ERROR"
				;;

		esac
		echo "<div class='result'><status>"${status}"</status></div>"
	else
		echo "<div class='result'><status>TIMEDOUT</status></div>"
	fi
?>
<? bweb_footer ?>
