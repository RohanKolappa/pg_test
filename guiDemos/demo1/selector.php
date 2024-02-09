<!DOCTYPE html>
<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<html>
<head>
<script>

gSelectedSource = "";

function getSelectedSource() {
  return gSelectedSource;
}

function callback(list) {
	var selector = document.getElementById("sourceselector");
	for(var option in selector) {
	 	selector.remove(option);
	}
	
	for(var i = 0; i < list.length; i++) {
		var opt = document.createElement('option');
		opt.value = list[i]["@mediaURL"];
		opt.innerHTML = list[i]["@title"];
		selector.appendChild(opt);
	}
	handleSourceSelected();
}

function getSources() {
	// alert("getSources");
	if(this.top.opener != undefined)
		this.top.opener.getSources(callback);
	else {
		var resultStr = "{\"Resource\": {\"Port\":" +
		"[" +
		"{\"@mediaURL\": \"source.php?axis4\",\"@title\": \"axis4\"}," +
		"{\"@mediaURL\": \"source.php?ngsd220\",\"@title\": \"ngsd220\"}" +
		"]" +
		"}}";
		var obj = JSON.parse(resultStr);
		callback(obj.Resource.Port);
	}
}

function handleSourceSelected() {
	var selector = document.getElementById("sourceselector");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined)
		return;
	var mediaURL = opt.value;
	if(mediaURL == undefined)
		return;
	var sourceframe = document.getElementById("sourceframe");
	sourceframe.src = mediaURL;
	gSelectedSource = opt.innerHTML;
}

</script>
</head>
<body onload="getSources();">
<table>
<tr><td>
Encoder: <select id="sourceselector" size="1" style="min-width:20;" onChange="handleSourceSelected();">
</select>
<a href="javascript:getSources();" class="btn-basic btn-small">Refresh</a>
</td></tr>
<tr><td>
<!-- 
<iframe id="sourceframe" src='' scrolling='yes' width='400' height='200' frameborder='1' style='position:absolute;Left:0;top:0'></iframe>
-->
<iframe id="sourceframe" src='' scrolling='yes' width='100%' frameborder='1'></iframe>
</td></tr>
</table>
</body>
</html>
