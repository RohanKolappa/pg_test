#!/usr/bin/haserl
<?
source /usr/share/bweb/localui/bweb_localui.sh
PARAMS="/tmp/fuse/params"

bweb_header 1

?>
  <script type="text/javascript">
  $(document).ready(function(){
	$("#uploadform").validate();
  });
  </script>
  <style type="text/css">
	#uploadform .error { color: red }
  </style>
<h1 class="title">System logs</h1>
<br>

<table width="600" >

<? for i in `ls /var/log`; do ?>
   <tr><td><h2><? echo -n /var/log/$i ?></h2></td></tr>
   <tr><td><pre><? cat /var/log/$i ?></pre></td></tr>
<? done ?>

</table>

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
<?
fi

bweb_footer
?>
