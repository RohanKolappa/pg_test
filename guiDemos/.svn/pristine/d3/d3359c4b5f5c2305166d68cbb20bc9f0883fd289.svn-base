<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<!DOCTYPE html>
<html>
<head>
<link href="css/barco-nd-layout.css" rel="stylesheet" type="text/css" />
<link href="css/barco-nd-general.css" rel="stylesheet" type="text/css" />

<script>

gWaitCount = 0;
gLinkURL = "";

function waitForShareURL() {
	var layoutURL = window.opener.getWaitURL();
	if(layoutURL == "") {
		gWaitCount ++;
//		alert("waitForShareURL waiting=" + gWaitCount);
		setTimeout(waitForShareURL, 1000);
	}
	else {
//		alert("waitForShareURL done layoutURL=" + layoutURL);
		gLinkURL = layoutURL;
		showShareURL();
	}
}

function showShareURL() {
	gLinkURL = gLinkURL + "&confOpt_playerPref=HLS";
	var linkURL = document.getElementById("linkURL");
	linkURL.value = gLinkURL;
	var linktag = "<a href=\"mailto:someone@example.com?subject=Shared-link&body=" + encodeURIComponent(gLinkURL) + "\">Mail</a>";
	document.getElementById("email").innerHTML = linktag; 
	// window.location = shareURL;
}


</script>
</head>


<body>
<textarea id="linkURL" rows="10" cols="50" readonly="readonly">Status</textarea>
<br/>
Copy/paste the URL above or use the Mail link below
<br>
<br>
<center>
<a href="javascript:close();" class="btn-basic btn-small">Close</a>
<label id="email" class="btn-primary">Generating URL</label>
</center>
<?php if( ($_SERVER['QUERY_STRING'] == "WAIT") || ($_SERVER['QUERY_STRING'] == "PUBLISH") ) : ?>
<script>
waitForShareURL();
</script>
<?php else : ?>
<script>
<?php 
	$query_string = $_SERVER['QUERY_STRING'];
	$mediaURL = base64_decode($query_string);
	print "gLinkURL='".$mediaURL."';\n";
?>
showShareURL();
</script>
<?php endif; ?>
<!--
<script>
gLinkURL = "http://www.barco.com";
showShareURL();
</script>
-->

</body>
</html>
