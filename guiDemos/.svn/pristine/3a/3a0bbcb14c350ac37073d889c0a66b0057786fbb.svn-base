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

<body>

<div class="header row">
	<div class="page-subtitle">MediaBar</div>
</div><!-- End Header -->

<form name="mrform">
<input type="hidden" name="resource" value="web-client" size="10"/>
<input type="hidden" name="service" value="2.32.3" size="10"/>
<input type="hidden" name="accept" value="application/json" size="150"/>
<input type="hidden" name="user" value="barco" size="10"/>
<input type="hidden" name="password" value="b4rc0,BCD" size="10"/>

<div class="body row scroll-y"> <!-- Body Panel -->

<div class="left col"> <!-- Center Panel -->



<!-- Select Sources, Webs, Layouts --->
<div class="card" style="width:280px; margin:10px 0 10px 15px;">
<table width="260" border="0" rules=none>
<tr><td colspan="2">
<select id="menu" size="1" style="min-width:50%;" onChange="handleMenuSelected();">
<option value="/controller/Resource/Port?selectFormat=BASICURL&sortBy=title&type=SrcPort">Source</option>
<option value="/controller/Resource/File?selectFormat=BASICURL&sortBy=title">Recording</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_web_">Web</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_layout_">Layout</option>
<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_tfn_">TFN</option>
</select>
<a href="javascript:handleMenuSelected();" class="btn-basic btn-small">Refresh</a>
<a href="javascript:handleAddCurrentListType();" class="btn-primary btn-small">Add</a>
</td></tr>
<tr><td colspan="2"><label id="info">Please Login</label></td></tr>
<tr><td colspan="2">
<select id="selector" size="10" style="min-width:90%;" onclick="handleMediaSelected();">
</select>
</td></tr>
<tr><td>
Mode:<select id="mode" size="1" onchange="handleModeSelected()">
<option value="view">View</option>
<option value="share">Share</option>
<option value="embed">Embed</option>
<option value="publish">Publish</option>
<option value="rename">Rename</option>
</select>
</td>
<td>
<span id="spanExp" style="display:none;">Expiry:&nbsp;&nbsp;<input type="text" id="txtExpTime" value="6" size="1">hrs</span>
<span id="spanVisited" style="display:block;">Visited:<label id="showing">0</label> of <label id="total">0</label></span>
<!-- Visited:<label id="showing">0</label> of <label id="total">0</label></td></tr> -->
</td></tr>
</table>
<br>
<center>
<a href="javascript:handleCancel();" class="btn-basic">Cancel</a>
<a href="javascript:handleOK(document.mrform.user.value,document.mrform.password.value,document.mrform.resource.value, document.mrform.service.value);" class="btn-primary">Login</a>
</center>
</div>
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
