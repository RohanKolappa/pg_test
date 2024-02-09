#!/usr/bin/haserl -u 10000
<?
source /usr/share/bweb/wizard/bweb_wizard.sh
#bweb_header 

?>

<?
echo "Step 1" >> /tmp/megm.log
bweb_cli
echo "Step 2" >> /tmp/megm.log

/V2O/bin/save_config
echo "Step 3" >> /tmp/megm.log

?>
<?
	local status="OK"
	if [ x"${FORM_board_info_boardtype}" = "xStreamDst" ]; then
		status="REBOOT"
		echo "<div class='result'><status>${status}</status></div>"
        /V2O/bin/reboot_system
	else
		echo "<div class='result'><status>${status}</status></div>"
	fi
?>
<? 
#bweb_footer 
?>
