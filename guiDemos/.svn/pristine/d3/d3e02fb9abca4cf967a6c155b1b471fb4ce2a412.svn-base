<!DOCTYPE html>
<html>
<head>
<meta charset="ISO-8859-1">
<title>MediaBar</title>
<!-- Barco CSS Files -->

<link href="css/barco-nd-layout.css" rel="stylesheet" type="text/css" />
<link href="css/barco-nd-general.css" rel="stylesheet" type="text/css" />

</head>

<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<script type="text/javascript" src="index.js">
</script>

<body onbeforeunload="handleCancel()">

<div class="header row">
	<div class="page-subtitle">MediaBar</div>
</div><!-- End Header -->

<form name="mrform">
<input type="hidden" name="resource" value="web-client" size="10"/>
<input type="hidden" name="service" value="2.32.3" size="10"/>
<input type="hidden" name="accept" value="application/json" size="150"/></td></tr>

<div class="body row scroll-y"> <!-- Body Panel -->

<div class="left col"> <!-- Center Panel -->

<?php if($_SERVER['QUERY_STRING'] == "") : ?>

<!-- Login --->
<div class="card" style="width:280px; margin:20px 0 10px 15px;padding-top:5px">
<div class="text-center" style="margin-bottom:5px;">
<h5 class="t-primary text-center">Login</h5></div>
<table width="100%" id="table1"  border="0" rules=none>
<tr><td><input type="text" class="form-control" name="user" value="barco" size="10"/></td></tr>
<tr><td><input type="password" class="form-control" name="password" value="" size="10"/></td></tr>
<tr><td>
<div style="text-align:right;">
<a href="javascript:handleCancel();" class="btn-basic">Cancel</a>
<a href="javascript:handleOK(document.mrform.user.value,document.mrform.password.value,document.mrform.resource.value, document.mrform.service.value);" class="btn-primary">Login</a>
</div>
</td></tr>
</table>
</div>

<?php else : ?>
<script>
<?php
	$query_string = $_SERVER['QUERY_STRING'];
	print "updateLogin('".$query_string."',document.mrform.service.value);\n";	
?>
</script>
<?php endif; ?>


<!-- Select Sources, Webs, Layouts --->
<div class="card" style="width:280px; margin:10px 0 10px 15px;">
<table width="260" border="0" rules=none>
<tr><td>
<select id="menu" size="1" style="min-width:50%;" onChange="handleMenuSelected();">
<option value="/controller/Resource/Port?selectFormat=BASICURL&sortBy=title&type=SrcPort">Source</option>
<option value="/controller/Resource/File?selectFormat=BASICURL&sortBy=title">Recording</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_web_">Web</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_layout_">Layout</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_tfn_">TFN</option>
</select>
<a href="javascript:handleMenuSelected();" class="btn-basic btn-small">Refresh</a>
<?php if($_SERVER['QUERY_STRING'] == "") : ?>
<a href="javascript:handleAddCurrentListType();" class="btn-primary btn-small">Add</a>
<?php else : ?>
<?php endif; ?>
</td></tr>
<tr><td><label id="info">Please Login</label></td></tr>
<tr><td>
<select id="selector" size="10" style="min-width:90%;" onclick="handleMediaSelected();">
</select>
</td></tr>
<tr><td>
Mode:<select id="mode" size="1">
<option value="view">View</option>
<option value="share">Share</option>
<option value="embed">Embed</option>
<option value="rename">Rename</option>
</select>
Visited:<label id="showing">0</label> of <label id="total">0</label></td></tr>
</table>
</div>


<?php if($_SERVER['QUERY_STRING'] == "") : ?>

<!-- Select Servers --->
<div class="card" style="width:280px; margin:10px 0 10px 15px;">
<table width="260" border="0" rules=none>
<tr><td>
<select id="servermenu" size="1" style="min-width:50%;" onChange="handleServerMenuSelected();">
<option value="/controller/Resource/Device?whereTextSubStr=_ms_">MS</option>
<option value="/controller/Resource/Device?whereTextSubStr=_node_">Node</option>
<!--
<option value="/controller/Resource/Device?whereTextSubStr=_dior_">DIOR</option>
-->
</select>
<a href="javascript:handleServerMenuSelected();" class="btn-basic btn-small">Refresh</a>
<a href="javascript:handleAddCurrentServerListType();" class="btn-primary btn-small">Add</a>
</td></tr>
<tr><td>
<select id="serverselector" size="2" style="min-width:90%;" onclick="handleServerSelected();">
</select>
</td></tr>
</table>
</div>

<?php else : ?>
<?php endif; ?>

<textarea id="response" name="response" rows="1" cols="100%" readonly="readonly" style="background-color: gray">Status</textarea><br/>
<!-- 
<textarea id="response" name="response" rows="40" cols="60"></textarea><br/>
-->
<!-- PrivateData --->
<!--
MyData: <input type="text"  name="privatedata" value="privatedata" size="20"/>
<a href="javascript:putPrivateData(document.mrform.privatedata.value);" class="btn-basic btn-small">Put</a>
<a href="javascript:getPrivateData(testCB);" class="btn-basic btn-small">Get</a>
-->
</div><!-- End Center Panel -->



</div><!-- End Body Panel -->

</body>
</html>
</select>
