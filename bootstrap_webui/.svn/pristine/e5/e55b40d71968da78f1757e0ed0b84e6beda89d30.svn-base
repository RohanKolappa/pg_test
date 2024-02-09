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
	<h1 class="title">KB/M Diagnostics</h1>
	<? 
	if [ $bweb_auth_level -ge 1 ]; then
		      bweb_getboardtype
        if [ x"$boardtype" = "xDEC" ]; then
	       bweb_listdata get_rx_display_io_info_status "Decoder KB/M Diagnostic Information"
	    else
	       bweb_listdata get_tx_display_io_info_status "Encoder KB/M Diagnostic Information"
	    fi
	 fi
	?>
		                                                                                                                                              
	<h1 class="title">Keyboard Locale Settings</h1>
	<?                              
		bweb_getboardtype         
		if [ $bweb_auth_level -ge 1 ]; then
            if [ x"$boardtype" = "xDEC" ]; then
			bweb_getconfig rx_kbm_layout
			fi
            if [ x"$boardtype" = "xENC" ]; then
			bweb_getconfig tx_kbm_layout
			fi
			bweb_addbuttons 
		fi
	?>
	</form>

<? bweb_footer ?>
