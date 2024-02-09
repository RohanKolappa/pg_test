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
	<h1 class="title">Audio Settings</h1>
	<?                              
		bweb_getboardtype         
		if [ $bweb_auth_level -ge 1 ]; then
			bweb_getconfig xp_audio_mixer
			bweb_addbuttons 
		fi
	?>
	</form>

<? bweb_footer ?>
