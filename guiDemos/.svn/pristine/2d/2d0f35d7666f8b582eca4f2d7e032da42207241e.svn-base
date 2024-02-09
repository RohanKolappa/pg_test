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
	<link href="css/demo3.css" rel="stylesheet" type="text/css" />

<script>

function getWebList() {
	var username = "barco";
	var password = "b4rc0,BCD";
	var resource = "web-client";
	var mainFrame = getMainFrame();
	mainFrame.gAuthorization = mainFrame.Base64.encode(username+":"+password);
	mainFrame.gResource = resource;
	// gResource = resource+"_"+username;
	
	var path ="/controller/Resource/Device?sortBy=title&whereTextSubStr=_web_";	
	mainFrame.sendAPIRequest(path, "Web", handleWebList, "");
	
}

function getMainFrame() {
  var mainFrame = document.getElementById('main').contentWindow;
  return mainFrame;
}


function handlePerspectiveSelected(id) {
  var url = getMainFrame().Base64.decode(id);
  document.getElementById('player').src = url;
}

function handleWebList(request, listType, cbdata) { 
	var obj = JSON.parse(request.responseText);
	var	list = obj.Resource.Device;
	// var list = getMainFrame().convertToList(devicelist);
	var weblist = [];
	for (var i = 0; i < list.length; i++) {
	  var id = getMainFrame().getValue(listType, list[i]);
	  var name = list[i]["@title"];
	  var item = {"name":name, "id": id};
	  weblist[weblist.length] = item;
	}
	var obj = {"RSource":weblist};
	document.getElementById('pselector').contentWindow.handleList(obj, handlePerspectiveSelected);
}

function initWebList() {
	window.setTimeout(function(){
			getWebList();
		}, 500);
	
}

</script>
</head>


<body onload="javascript:initWebList();">
<div data-role="panel" data-position="left" data-display="overlay" id="mediaBar" data-theme="a">
    <ul data-role="listview" data-theme="d">
        	<li data-icon="delete"><a href="#" data-rel="close">Close</a></li>
    </ul>
    <ul data-role="listview">
  		<li data-role="collapsible" data-iconpos="right" data-inset="false">
			<h2>Perspectives</h2>
				<iframe width="0" height="0" id="main" src="../demo1/demo3.php?mode=WEB" frameborder="0"></iframe>
			<div id="divSideBarBottom"  style="width:320px;height:500px;">
				<iframe  width="100%" height="100%" id="pselector" src="pselector.php" frameborder="0"></iframe>
			</div>
  		</li>
  		<li data-role="collapsible" data-iconpos="right" data-inset="false" data-disabled="true">
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