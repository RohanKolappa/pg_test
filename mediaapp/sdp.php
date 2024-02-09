<?php
    handleHttpRequest();
?>

<?php

function handleHttpRequest() {
    ob_start();
    $url = curPageURL();

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        return doGet($url, $queryStringArray);
    }
}

?>

<?php

function curPageURL() {
    $pageURL = 'http';
    if ($_SERVER["HTTPS"] == "on") {
        $pageURL .= "s";
    }
    $pageURL .= "://";
    if ($_SERVER["SERVER_PORT"] != "80") {
        $pageURL .= $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
    }
    else {
        $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
    }

    return $pageURL;
}

function doGet($url, $queryStringArray) {
    $base_dir = substr($url, 0, -4);

    $base_pos = strripos($url, "/", 0);
    $index_base_name = substr($url, $base_pos + 1, -4);
    $index_file_path = '/home/onair/modules/mediaroom/public' .'/'.$index_base_name.'.sdp';
    #echo $index_file_path;
  
 start: 
    $timeForIndex = 0;
    $timeForURL = 0;
    $bFoundFile = 0;
    
    do {
        clearstatcache(true);
        if ((file_exists($index_file_path)) || (is_readable($index_file_path)) ) {
            $bFoundFile = TRUE;
            break;
        }
        else {
            sleep(1);
            $timeForIndex += 1;
            $bFoundFile = FALSE;
        }
    } while ($timeForIndex < 5);    

    if ($bFoundFile == FALSE) {
        header('HTTP/1.0 415 Invalid Media Path',true,415);
        echo "Invalid media path";
        return; 
    }
		// Do HTTP Request to device Agent
	$xml_data = '<SSEvent  id=\'' . $index_base_name . '\' state=\'Connected\'></SSEvent>';
	// echo htmlentities($xml_data, ENT_COMPAT, 'UTF-8');
	$URL = "http://localhost:10080/deviceinterface/Media";
	
	$ch = curl_init ( $URL );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_setopt ( $ch, CURLOPT_CUSTOMREQUEST, "POST" );
	curl_setopt ( $ch, CURLOPT_HTTPHEADER, array (
			'Content-Type: text/xml' 
	) );
	curl_setopt ( $ch, CURLOPT_POSTFIELDS, "$xml_data" );
	curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, 1 );
	
	$output = curl_exec ( $ch );
    

    $index_file_content = file_get_contents($index_file_path);

    header ( 'Content-Type:application/text' );
    
    echo $index_file_content;
}

?>
