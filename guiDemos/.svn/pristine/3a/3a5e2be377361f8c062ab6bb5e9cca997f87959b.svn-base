<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="ISO-8859-1">
<title>Add Dialog</title>
<link href="css/barco-nd-layout.css" rel="stylesheet" type="text/css" />
<link href="css/barco-nd-general.css" rel="stylesheet" type="text/css" />
</head>
<script>

var gObjectType = "";

function getOptionValue(menu) {
	var addselector = document.getElementById(menu);
	var opt = addselector.options[addselector.selectedIndex];
	return opt.value;
}

<!-- Used to update the layout html for grid mode by generating x,y,width,height for each frame -->

function updateLayout(addvalue, rows) {
	var canvasType = getOptionValue("canvastypemenu");
	if(canvasType == "custom")
		return addvalue;
	var lines = addvalue.split("\n");
	var newvalue = "";
	var maxcols = getOptionValue("colsmenu");
	var row = 0;
	var col = 0;
	var width = getOptionValue("widthmenu");
	var height = getOptionValue("heightmenu");
	
	var el = document.createElement("div");
	el.innerHTML = addvalue;
	var list = el.getElementsByTagName('iframe');
	var sourceList = "";
	for(var i = 0; i < list.length; i++) {
		var xposition = col * width;
		var yposition = row * height;
		list[i].width = width;
		list[i].height = height;
		list[i].style.position = 'absolute';
		list[i].style.left = xposition.toString() + 'px';
		list[i].style.top = yposition.toString() + 'px';
		col++;
		if(col == maxcols) {
			col = 0;
			row++;
		}
	} 
	return el.innerHTML;
}

<!-- Used to update the layout -->

function handleOK() {
	var addtitle = document.getElementById("addtitle1").value;
	var addvalue = document.getElementById("addvalue1").value;
	
	if(gObjectType == "Layout")
		addvalue = updateLayout(addvalue);

	var addselector = document.getElementById("add1");
	var opt = addselector.options[addselector.selectedIndex];
	// alert("type=" + gObjectType + "\ntitle=" + addtitle + "\nvalue=" + addvalue);
	window.opener.handleAddCallback(gObjectType, addtitle, addvalue);
	handleCloseAddWindow();
}


function handleCloseAddWindow() {
	window.close();
}

<!-- used to populate the list of system generated options for the object being added -->

function handleAddChanged1() {
	var addselector = document.getElementById("add1");
	var opt = addselector.options[addselector.selectedIndex];
	document.getElementById("addtitle1").value = opt.innerHTML.toLowerCase();
	document.getElementById("addvalue1").value = opt.value;
}

<?php 
	if(substr($_SERVER['QUERY_STRING'], 0, strlen("Layout")) == "Layout")
		print "gObjectType='Layout'";
	else
		print "gObjectType=\"".$_SERVER['QUERY_STRING']."\"";
	
?>

</script>



<body onload="handleAddChanged1()">
<form name="mrform">
<table>
<tr><td>Use</td>
<td><select id="add1" size="1" onclick="handleAddChanged1();">

<!-- Source --->

<?php if($_SERVER['QUERY_STRING'] == "Source") : ?>
<option value='v=0\no=- 1041039156762093 1041039156762093 IN IP4 10.51.49.46\ns=Media Presentation\ne=NONE\nb=AS:50032\nt=0 0\na=control:*\na=range:npt=0.000000-\nm=video 3000 RTP/AVP 96\nc=IN IP4 224.1.155.1\nb=AS:50000\na=framerate:30.0\na=transform:1,0,0;0,1,0;0,0,1\na=control:trackID=1\na=rtpmap:96 H264/90000\na=fmtp:96 packetization-mode=1; profile-level-id=4D0029; sprop-parameter-sets=Z00AKeKQFge2BqwYBBuHiRFQ,aO48gA==\nm=audio 3002 RTP/AVP 97\nc=IN IP4 224.1.155.1\nb=AS:32\na=control:trackID=2\na=rtpmap:97 mpeg4-generic/16000/1\na=fmtp:97 streamtype=5; profile-level-id=15; mode=AAC-hbr; config=1408; sizeLength=13; indexLength=3; indexDeltaLength=3; profile=1; bitrate=32000;'>sample-source</option>";
<?php

	// Reads the list of files in the sap directory to populate the list of SAP discovered SDPs
	
	$path = "/data/demos/sap";
	
	if(is_dir($path)) {
		if($dh = opendir($path)) {
			while(($file = readdir($dh)) !== false) {
				if(($file == ".")  || ($file == ".."))
					continue;
				print "<option value=\"" ;
				$handle = fopen($path . "/" . $file, "r");
				while( ($line = fgets($handle)) != false)	{
					$line = str_replace("\\", "", $line); // for NGS-D220
					print "$line";
				}
				print "\">sdp-".$file."</option>";
			}
			closedir($dh);
		}
	}
	
?>
<?php else : ?>
<?php endif; ?>

<!-- Web --->

<?php if($_SERVER['QUERY_STRING'] == "Web") : ?>
<option value="http://www.barco.com">sample-link</option>
<?php else : ?>
<?php endif; ?>

<!-- Node -->
<?php if($_SERVER['QUERY_STRING'] == "Node") : ?>
<option value="http://www.barco.com">sample-node</option>
<?php

	// Reads the list of files in the zc directory to populate the list of Zero conf discovered node admin URLs
	
	$path = "/data/demos/zc";
	
	if(is_dir($path)) {
		if($dh = opendir($path)) {
			while(($file = readdir($dh)) !== false) {
				if(($file == ".")  || ($file == ".."))
					continue;
				print "<option value=\"" ;
				$handle = fopen($path . "/" . $file, "r");
				while( ($line = fgets($handle)) != false)	
					print trim($line);
				print "\">".$file."</option>";
			}
			closedir($dh);
		}
	}
	
?>
<?php else : ?>
<?php endif; ?>

<!-- Layout --->

<?php if(substr($_SERVER['QUERY_STRING'], 0, strlen("Layout")) == "Layout") : ?>
<option value=''>sample-layout</option>
<?php else : ?>
<?php endif; ?>

<?php if($_SERVER['QUERY_STRING'] == "MS") : ?>
<option value="10.51.51.38 barco b4rc0,BCD web-client">sample-ms</option>
<?php else : ?>
<?php endif; ?>

</select>
</td></tr>


<!-- Input --->
<tr><td>Title</td><td><input id="addtitle1" type="text" name="addtitle1" size="40" value=""/></td></tr>
<!-- Layout Media -->

<?php if(substr($_SERVER['QUERY_STRING'], 0, strlen("Layout")) == "Layout") : ?>
<script>
<!-- Used to generate a frame tag using the source and add to the html -->

function handleAddTag() {
	var selector = document.getElementById("layoutmedia");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined)
		return;
	var tag = opt.value;
	var addvalue = document.getElementById("addvalue1");
	addvalue.value = addvalue.value + "\n" + tag;
}
</script>

<tr><td>Canvas</td>
<td>
<select id="canvastypemenu" size="1">
<option value="grid">Grid</option>
<option value="custom">Custom</option>
</select>
Cols
<select id="colsmenu" size="1">
<option value="1">1</option>
<option value="2">2</option>
<option value="3">3</option>
<option value="4">4</option>
</select>
CellWidth
<select id="widthmenu" size="1">
<option value="400">400</option>
<option value="500">500</option>
<option value="600">600</option>
<option value="700">700</option>
</select>
CellHeight
<select id="heightmenu" size="1">
<option value="400">400</option>
<option value="500">500</option>
<option value="600">600</option>
<option value="700">700</option>
</select>
</td></tr>

<tr><td>Media</td>
<td><select id="layoutmedia" size="1">
<?php
	// The medialist is passed in the query string and used to populate the media options
	$mediaListStr = substr($_SERVER['QUERY_STRING'], strlen("Layout="));
	$mediaList = explode(",", $mediaListStr);
	for($i = 0; $i < count($mediaList) -1; $i++) {
		$title = $mediaList[$i];
		$value = "<iframe src='source.php?".$title."' scrolling='yes' width='_WIDTH_' height='_HEIGHT_' frameborder='0' style='position:absolute;Left:_XPOSITION_;top:_YPOSITION_'></iframe>";
		print "<option value=\"".$value."\">".$title."</option>";
	}
?>
</select>
<a href="javascript:handleAddTag();" class="btn-primary">AddTag</a>
</td></tr>



<?php else : ?>
<?php endif; ?>
<!--- --->
</table>
<textarea id="addvalue1" name="addvalue1" rows="19" value="" style='width:100%;'></textarea>
<br>
<center>
<a href="javascript:handleOK();" class="btn-primary">OK</a>
<a href="javascript:handleCloseAddWindow();" class="btn-basic">Cancel</a>
</center>
</form>
</body>
