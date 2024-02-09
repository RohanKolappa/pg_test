#!/usr/bin/haserl
<?
source /usr/share/bweb/localui/bweb_localui.sh

bweb_header 1

?>
  <style type="text/css">
	#uploadform .error { color: red }
  </style>
<form class="form-panel" method="post" >
<h1 class="title">Discovered Devices</h1>
<br>
<table class="formTable">
<tr><th>Name</th><th>IP Address</th></tr>
<?
	for i in `avahi-browse -t _https._tcp|awk '{print $5}'`; do
	    a=`avahi-resolve-host-name $i.local`
	    name=$i.local
	    address=`echo $a | awk '{print $2}'`
	    ?>
	    <tr><td class="table-label">
	    <a href="http://<? echo -n $address ?>"><? echo -n $name ?></a>
	    </td><td>
	    <? echo -n $address ?>
	    </td>
	    </tr>
	    <?
	done
?>
</table>
</form>
<script type="text/javascript">
	$(document).ready(function() {
		d = $("#dialog").dialog({
			bgiframe: true,
			modal: true,
			draggable: false,
			resizable: false,
			autoOpen: false,
			show: 'scale',
			hide: 'scale',
			buttons: {
				Ok: function() {
					$(this).dialog('close');
				}
			}
		});
		// get rid of close button
		$('.ui-dialog-titlebar-close').empty()
		d.dialog('open');
	});
</script>
<? bweb_footer ?>
