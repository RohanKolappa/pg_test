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
	<h1 class="title">Encoding Settings</h1>
	<?                              
		bweb_getboardtype         
		if [ x"$boardtype" = "xENC" ]; then
			if [ $bweb_auth_level -ge 1 ]; then
				bweb_getconfig tx_connection_parm
				bweb_getconfig tx_rfb_parm
				bweb_addbuttons 
			fi
		fi
	?>
	</form>

<? bweb_footer ?>
