<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<html>
<head>
<script>

function callback(url, cbdata) {
  // alert("callback url=" + url);
  window.location = url;
}

function getURL() {
<?php
  print "  var title='".$_SERVER['QUERY_STRING']."';\n";
?>
  // alert("getURL title=" + title);
  var cbdata = "";
  this.top.opener.getURL(title, callback, cbdata, "");
}
</script>
</head>
<body>
<script>
getURL();
</script>
</body>
</html>
