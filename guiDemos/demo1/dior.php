<?php

function getSOAPResponse($url, $post_string) {
  $soap_do = curl_init(); 
  curl_setopt($soap_do, CURLOPT_URL,            $url );   
  curl_setopt($soap_do, CURLOPT_CONNECTTIMEOUT, 10); 
  curl_setopt($soap_do, CURLOPT_TIMEOUT,        10); 
  curl_setopt($soap_do, CURLOPT_RETURNTRANSFER, true );
  curl_setopt($soap_do, CURLOPT_SSL_VERIFYPEER, false);  
  curl_setopt($soap_do, CURLOPT_SSL_VERIFYHOST, false); 
  curl_setopt($soap_do, CURLOPT_POST,           true ); 
  curl_setopt($soap_do, CURLOPT_POSTFIELDS,    $post_string); 
  curl_setopt($soap_do, CURLOPT_HTTPHEADER,     array('Content-Type: text/xml; charset=utf-8', 'Content-Length: '.strlen($post_string) )); 
  // curl_setopt($soap_do, CURLOPT_USERPWD, $user . ":" . $password);

  $result = curl_exec($soap_do);
  $err = curl_error($soap_do);  
  return $result;
}

function getJSON($result, $resource) {
  $startindex = strpos($result, "<return>");
  $endindex = strpos($result, "</return>") + strlen("</return>");
  $xmlstring = substr($result, $startindex, $endindex - $startindex);
  $xml = new SimpleXMLElement($xmlstring); 

  $json = '{"'.$resource.'":['."\n";

  // Debug
  // print "<pre>"; var_dump($xml); print "</pre>";

  $count = 0;
  foreach($xml->item as $item) {
    if($count > 0)
      $json = $json.",";
    $count++;
    if($resource == "Display")
      $json = $json.'{"name":"'.$item->displayInfo->name.'", "id":"'.$item->displayId->id.'"}'."\n";
    else if($resource == "RSource")
      $json = $json.'{"name":"'.$item->perspectiveinfo->name.'", "id":"'.$item->perspectiveId->id.'"}'."\n";
  }
  $json = $json.']}';
  return $json;
}

error_reporting(E_ALL);
ini_set('display_errors', 1);

parse_str($_SERVER['QUERY_STRING']);
// This should initialize $ip and $resource

$url = "http://".$IP."/cms/PerspectiveMgtAPIImpl?wsdl";
// $url = "http://10.51.32.8/~miyer/cms.php";

// Get Displays
if($resource == "Display") {
  $post_string = "<soap:Envelope xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:xsd='http://www.w3.org/2001/XMLSchema' xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'><soap:Body><n:listAllDisplays xmlns:n='http://openapi.cms.barco.com/'><arg0 xsi:type='tns:sessionId'><id>1</id> </arg0></n:listAllDisplays></soap:Body></soap:Envelope>";
}
else if($resource == "RSource") {
  $post_string = "<soap:Envelope xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:xsd='http://www.w3.org/2001/XMLSchema' xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'><soap:Body><n:listAllPerspectives xmlns:n='http://openapi.cms.barco.com/'><arg0 xsi:type='tns:sessionId'><id>1</id> </arg0></n:listAllPerspectives></soap:Body></soap:Envelope>";
}
else { // Has to be share
  parse_str($_SERVER['QUERY_STRING']);
  $post_string = "<soap:Envelope xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:xsd='http://www.w3.org/2001/XMLSchema' xmlns:soap='http://schemas.xmlsoap.org/soap/envelope/'><soap:Body><n:sharePerspectiveWithDefaultGeometry xmlns:n='http://openapi.cms.barco.com/'><arg0 xsi:type='tns:sessionId'><id>1</id></arg0><arg1 xsi:type='tns:objectId'><id>".$Display."</id></arg1><arg2 xsi:type='tns:objectId'><id>".$RSource."</id></arg2></n:sharePerspectiveWithDefaultGeometry></soap:Body></soap:Envelope>";
}

$result = getSOAPResponse($url , $post_string);
$json = "{\"Status\":\"OK\"}";
if( ($resource == "Display") || ($resource == "RSource") ) {
  $json = getJSON($result, $resource);
}
header("Content-Type: text/html; charset=UTF-8");
echo($json);
// echo json_encode($json);
// header('Content-Type: application/json');
// echo json_encode($json);

?>
