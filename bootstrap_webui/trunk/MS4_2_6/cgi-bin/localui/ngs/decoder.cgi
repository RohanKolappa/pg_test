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
	<h1 class="title">Decoding Settings</h1>
	<?                              
		bweb_getboardtype         
		if [ x"$boardtype" = "xDEC" ]; then
				bweb_getconfig display_parm
				bweb_addbuttons 
		fi                                                          
	?>               
	</form>

<? bweb_footer ?>
