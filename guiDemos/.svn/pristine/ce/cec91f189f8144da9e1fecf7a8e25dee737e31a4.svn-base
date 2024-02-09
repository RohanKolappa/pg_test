<!DOCTYPE html>
<html>
<head>

<script>
var gCallback;
function handleList(list, callback) {
  gCallback = callback;
  if (list && list.RSource) {
	var mHTML = "<ul data-role=\"listview\">";
	for (var i = 0; i < list.RSource.length; i++) {
	  var txt = list.RSource[i].name;
	  var itemId = list.RSource[i].id;
	  mHTML += "<li id='" + itemId + "' onclick=\"onItemSelected('" + itemId + "')\">" + txt + "</li>";
	}
	 mHTML += "</ul>";
	 document.getElementById("prospList").innerHTML = mHTML;
  }
}

function onItemSelected(param1) {
  gCallback(param1);
}
</script>
<style type="text/css">
<!--
body {
	padding:0;
	margin:0;
}
.perlist ul{
list-style-type: none;
margin: 0;
padding: 0;
margin-bottom: 0; /*bottom spacing between each UL and rest of content*/
}

.perlist ul li{
font: normal 14px Arial;
color: black;
display: block;
padding-left: 16px;
line-height: 30px;
text-decoration: none;
}
.perlist ul li:hover {
background: #f5f5f5;
}

.perlist ul li a{
font: normal 12px Arial;
color: black;
background: #E9E9E9;
display: block;
padding: 5px 0;
line-height: 17px;
padding-left: 8px; /*link text is indented 8px*/
text-decoration: none;
}

.perlist ul li a:visited{
color: black;
}
-->
</style>
</head>
<body>
	<div id="mainPanel" class="perlist">	
		<div id="prospList">
		</div>
	</div>
</body>
</html>