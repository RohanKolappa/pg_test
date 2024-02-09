<!DOCTYPE html>
<html>
<head>
	<meta http-equiv="X-UA-Compatible" content="IE=EDGE" />
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1,maximum-scale=1, user-scalable=no">
	<title>Control Room On The Go Demo</title>
	<script src="js/jquery-1.11.1.min.js"></script>
	<script src="js/jquery.mobile-1.4.4.min.js"></script>

	<link href="css/jquery.mobile-1.4.4.css" rel="stylesheet" type="text/css" />
    <link href="css/barco-jqm-theme.css.no" rel="stylesheet" type="text/css" />
  	<link href="css/jquery.mobile.icons.min.css" rel="stylesheet" type="text/css" />
	<link href="css/barco-jqm-overrides.css" rel="stylesheet" type="text/css" />
</head>

<script>
var gIP="";
var gDisplay="";

function getURLCallback(url, cbdata) {
  // alert("callback url=" + url);
  // top.frames["player"].src = url;
  document.getElementById('player').src = url;
}

function getMainFrame() {
  return document.getElementById('main').contentWindow;
}

function sendScriptRequest(path, callback) {
    var url = "../demo1/tfn.php?IP=" + gIP + path;
	var request = new XMLHttpRequest();

	function handler() {
		if (this.readyState == 4) {
	        if(this.status == 200) {
	        	callback(this.responseText);
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

function handlePerspectiveShared(resultStr) {
	// alert(resultStr);
}

function handlePerspectiveSelected(id) {
    var url = "&Display=" + gDisplay + "&RSource=" + id;
    sendScriptRequest(url, handlePerspectiveShared);
}

function handlePerspectives(pStr) {
	var obj = JSON.parse(pStr);
	document.getElementById('pselector').contentWindow.handleList(obj, handlePerspectiveSelected);
}

function handleSelected(url, wintitle) {

	var isTFN = url.indexOf("ObjectType=TFN");
	if(isTFN == -1) { // Not a TFN object
	  document.getElementById('player').src = url;
	  return;
	}
	
    // Load the player
    var startIndex = url.indexOf("Source=") + "Source=".length;
    var endIndex = url.indexOf("&Display=");
    var sourceURL = url.substr(startIndex, endIndex - startIndex);
    var mainFrame = getMainFrame();
    sourceURL = mainFrame.Base64.decode(sourceURL);
    var title = sourceURL.substring("source.php?".length);
    mainFrame.getURL(title, getURLCallback, url);

    var startIndex = url.indexOf("IP=") + "IP=".length;
    var endIndex = url.indexOf("&Source=");
    gIP = url.substr(startIndex, endIndex - startIndex);

    var startIndex = url.indexOf("Display=") + "Display=".length;
    gDisplay = url.substr(startIndex);
    
    // get the list of perspectives
    var path = "&resource=RSource";
    sendScriptRequest(path, handlePerspectives);
}

</script>
<body>

<div data-role="panel" data-position="left" data-display="overlay" id="mediaBar" data-theme="a">
    <!-- <ul data-role="listview" data-theme="d">
        	<li data-icon="delete"><a href="#" data-rel="close">Close</a></li>
    </ul> -->
    <ul data-role="listview">
  		<li data-role="collapsible" data-iconpos="right" data-inset="false" data-collapsed="false">
		<h2>View Ports</h2>
			<div id="divSideBarTop" style="width:100%;height:300px;">
				<iframe width="100%" height="100%" id="main" src="../demo1/demo3.php" frameborder="0"></iframe>
			</div>
		</li>
  		<li data-role="collapsible" data-iconpos="right" data-inset="false">
			<h2>Perspectives</h2>
			<div id="divSideBarBottom"  style="width:320px;height:500px;">
				<iframe  width="100%" height="100%" id="pselector" src="pselector.php" frameborder="0"></iframe>
			</div>
  		</li>

	</ul>
</div><!-- /panel -->



<div data-role="header" class="header">
        <h1>Media Agent Demo</h1>
        <a href="#mediaBar" class="ui-btn-left" data-icon="bars" data-iconpos="notext" data-shadow="false" data-iconshadow="false">Menu</a>
</div><!-- /header -->


<div id="divPlayer" class="player-container">
	<iframe width="100%" height="100%" id="player" frameborder="0"></iframe>
</div>
</body></html>