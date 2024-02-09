<!DOCTYPE html>
<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<html>
<head>
<meta charset="ISO-8859-1">
<title>Add Dialog</title>
<link href="css/barco-nd-layout.css" rel="stylesheet" type="text/css" />
<link href="css/barco-nd-general.css" rel="stylesheet" type="text/css" />
<script>

var gObjectType = "";
var gDisplayId = "";
var gIP = "";

var gBackground;
var gTimeout= null;
function hideMessage() {
    document.getElementById("refreshlabel").innerHTML =  "Refresh";
    document.getElementById("refreshlabel").style.background = gBackground;
    gTimeout = null;
}

function showMessage(message) {
	if(gTimeout != null) {
		clearTimeout(gTimeout);
	}
	else {
    	gBackground = document.getElementById("refreshlabel").style.background;
    }
    document.getElementById("refreshlabel").innerHTML = message;
    document.getElementById("refreshlabel").style.background = "yellow";
    document.getElementById('rsourceframe').src = document.getElementById('rsourceframe').src
	gTimeout = setTimeout(hideMessage,10000);
}

function sendScriptRequest(url, callback) {
    // Read the IP if not initialized
	if( (gIP == "") || (gIP == "0.0.0.0") ) {
	    gIP = document.getElementById("ip").value;
    }
    // Return if no valid ip found
	if( (gIP == "") || (gIP == "0.0.0.0") ) {
    	alert("Please set IP to point to the server");
    	return;
	}
    url = gObjectType.toLowerCase() +".php?IP=" + gIP + url
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
				var obj = JSON.parse(this.responseText);
	        	callback(obj);
	        }
        	else
		        error("Script request FAILED code=" + this.status + ":" + this.responseText);
		}
	}
  
   	request.onreadystatechange = handler;
	var async = true; // to prevent blocking execution
//	var contentType = "application/json;charset=UTF-8";
	var method = "GET";
	request.open(method, url, async);
//	request.setRequestHeader("Content-Type", contentType);
	request.send();
}

function handleOK() {
	var addtitle = document.getElementById("addtitle1").value;
	var ip = document.getElementById("ip").value;
	var selector = document.getElementById("displayselector");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined){
		alert("display not selected");
		return;
	}
	var displayId = opt.value;
	
	var sourcetitle = document.getElementById('selectorframe').contentWindow.getSelectedSource();
	// var sourcetitle = "axis4";
	if(sourcetitle == ""){
		alert("source not selected");
		return;
	}
	var srcURL = top.opener.Base64.encode("source.php?" + sourcetitle);
	var addvalue = "ObjectType=" + gObjectType + "&IP="+ ip + "&Source=" + srcURL  + "&Display=" + displayId;
	if(this.top.opener != undefined) {
		top.opener.handleAddCallback(gObjectType, addtitle, addvalue);
	}
	else {	
		alert(addvalue);
	}
	handleCloseAddWindow();
}


function handleCloseAddWindow() {
	window.close();
}

function displayCallback(obj) {
	var list = obj.Display;
	var selector = document.getElementById("displayselector");
	for(var option in selector) {
	 	selector.remove(option);
	}
	
	for(var i = 0; i < list.length; i++) {
		var opt = document.createElement('option');
		opt.value = list[i]["id"];
		opt.innerHTML = list[i]["name"];
		selector.appendChild(opt);
	}
}

function getDisplays() {
	sendScriptRequest("&resource=Display", displayCallback);
//	alert(url);
// 	var resultStr = "{\"Display\":[" +	 "{\"name\":\"Operator [SILCLT27125]\", \"id\":\"9\"}," + "{\"name\":\"Display [SILCLT27125]\", \"id\":\"1\"}" +"]}";
//	var obj = JSON.parse(resultStr);
//	displayCallback(obj.Display);
}

function rsourceCallback(obj) {
	var list = obj.RSource;
	var selector = document.getElementById("rsourceselector");
	for(var option in selector) {
	 	selector.remove(option);
	}
	
	// Add the reset element
	var opt = document.createElement('option');
	opt.value = "";
	opt.innerHTML = "Reset";
	selector.appendChild(opt);
		
	for(var i = 0; i < list.length; i++) {
		var opt = document.createElement('option');
		opt.value = list[i]["id"];
		opt.innerHTML = list[i]["name"];
		selector.appendChild(opt);
	}
}

function getRSources() {
	sendScriptRequest("&resource=RSource", rsourceCallback);
//	alert(url);
//	var resultStr = "{\"RSource\":[" +
//	 "{\"name\":\"NGS_101\", \"id\":\"2\"}, {\"name\":\"NGS4x4\", \"id\":\"4\"}]}";
//	var obj = JSON.parse(resultStr);
//	rsourceCallback(obj.RSource);
}

function rsourceSelectedCallback(obj) {
	// alert(obj);
}

function handleRSourceSelected() {
	var selector = document.getElementById("rsourceselector");
	var opt = selector.options[selector.selectedIndex];
	if(opt == undefined)
		return;
	var rsourceId = opt.value;
	if(rsourceId == undefined)
		return;
	var url = "&Display=" + gDisplayId + "&RSource=" + rsourceId;
	sendScriptRequest(url, rsourceSelectedCallback);
	showMessage("Please Wait ...");
}

</script>
</head>
<?php
$objectType = $_SERVER['QUERY_STRING'];
print "<script>gObjectType=\"".$objectType."\";</script>";
?>
<?php if( ($objectType == "TFN") || ($objectType == "DIOR") ) : ?>
<body>
<table>
<tr><td>Title</td><td><input id="addtitle1" type="text" name="addtitle1" size="20" value="sample-server"/></td></tr>
<tr><td>IP</td><td><input id="ip" type="text" name="ip" size="20" value="0.0.0.0"/></td></tr>
<tr>
<td>Display</td>
<td><select id="displayselector" size="1" style="min-width:20;">
</select>
<a href="javascript:getDisplays();" class="btn-basic btn-small">Refresh</a>
</td></tr>
</table>
<iframe id="selectorframe" src='selector.php' scrolling='yes' width='100%' height='250' frameborder='1'></iframe>
<br>
<center>
<a href="javascript:handleOK();" class="btn-primary">OK</a>
<a href="javascript:handleCloseAddWindow();" class="btn-basic">Cancel</a>
</center>
</form>
</body>
<?php else : ?>
<body onload="javascript:getRSources();">
<table>
<tr><td>
Perspective: <select id="rsourceselector" size="1" style="min-width:20;" onChange="handleRSourceSelected();">
</select>
<a href="javascript:getRSources();" class="btn-basic btn-small">
<span id="refreshlabel">Refresh</span>
</a>
</td></tr>
<tr><td>
<!-- 
<iframe id="rsourceframe" src='' scrolling='yes' width='400' height='200' frameborder='1' style='position:absolute;Left:0;top:0'></iframe>
-->
<?php
  parse_str($_SERVER['QUERY_STRING']);
  print "<script>\n";
  print "gObjectType=\"".$ObjectType."\";\n";
  print "gDisplayId=\"".$Display."\";\n";
  print "gIP=\"".$IP."\";\n";
  print "</script>\n";
  $Source = base64_decode($Source);
  print "<iframe id=\"rsourceframe\" src=\"".$Source."\" scrolling='yes' frameborder='1' width='100%' height='90%' style='position:absolute;Left:0;top:20'></iframe>";
?>
</td></tr>
</table>
</body>
<?php endif; ?>

</html>
