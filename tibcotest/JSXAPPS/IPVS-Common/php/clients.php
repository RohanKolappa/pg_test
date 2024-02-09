<?php
$app = $_GET['application'];
$loadframe="";
if ($app == "stx") {
  $loadframe ="GI/JSXAPPS/STX_ClientUI/index.php";
}
else if ($app == "ipvs") {
  $loadframe ="GI/IPVSClient-Index.php";
}
else if ($app == "admin") {
  $loadframe ="GI/JSXAPPS/IPVS_AdminUI/index.php";
}
else {
  $loadframe ="install/index.php";
}
header('Location:'.$loadframe);
?>
