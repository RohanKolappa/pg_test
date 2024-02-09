<!DOCTYPE html>
<html>
<head>
<meta charset="ISO-8859-1">
<title>MediaBar</title>
<!-- Barco CSS Files -->

<link href="css/barco-nd-layout.css" rel="stylesheet" type="text/css" />
<link href="css/barco-nd-general.css" rel="stylesheet" type="text/css" />
<style type="text/css">
<!--
body {
	padding:0;
	margin:0;
}
.selector ul{
list-style-type: none;
margin: 0;
padding: 0;
margin-bottom: 0; /*bottom spacing between each UL and rest of content*/
}

.selector ul li{
font: normal 14px Arial;
color: black;
display: block;
padding-left: 16px;
line-height: 30px;
text-decoration: none;
}
.selector ul li:hover {
background: #f5f5f5;
}

.selector ul li a{
font: normal 12px Arial;
color: black;
background: #E9E9E9;
display: block;
padding: 5px 0;
line-height: 17px;
padding-left: 8px; /*link text is indented 8px*/
text-decoration: none;
}

.selector ul li a:visited{
color: black;
}
-->
</style>
</head>

<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>

<script type="text/javascript" src="index3.js">
</script>

<body>
<form name="mrform">
<input type="hidden" name="resource" value="web-client" size="10"/>
<input type="hidden" name="service" value="2.32.3" size="10"/>
<input type="hidden" name="accept" value="application/json" size="150"/>
<input type="hidden" name="user" value="barco" size="10"/>
<input type="hidden" name="password" value="b4rc0,BCD" size="10"/>

<div style="display:none;">
	<select id="menu" size="1" style="min-width:50%;" onChange="handleMenuSelected();">
		<?php
		  parse_str($_SERVER['QUERY_STRING']);
		?>
		<?php if($mode == "WEB") : ?>
			<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_web_">Web</option>
		<?php else : ?>
			<option value="/controller/Resource/Device?sortBy=title&whereTextSubStr=_tfn_">TFN</option>
		<?php endif; ?>
	</select>
</div>
<div> <!-- Center Panel -->
	<div id="selector" class="selector">
    </div>

</div><!-- End Center Panel -->

<script>
	<?php if($mode != "WEB") : ?>
		window.setTimeout(function(){
			handleOK(document.mrform.user.value,document.mrform.password.value,document.mrform.resource.value, document.mrform.service.value);
		}, 1000);
	<?php else : ?>
		//do nothing
	<?php endif; ?>
</script>
</body>
</html>
</select>
