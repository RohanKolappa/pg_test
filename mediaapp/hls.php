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

function authenticateURL($queryStringArr) {
	$xml = simplexml_load_file ( "/var/status/APIKeyTable.xml" );
	$apiKey = $queryStringArr ["apiKey"];
	$doc = new DOMDocument ();
	$doc->load ( "/var/status/APIKeyTable.xml" );
	$apiKeyEls = $doc->getElementsByTagName ( 'ApiKey' );
	$sharedSecret = '';
	foreach ( $apiKeyEls as $apiKeyEl ) {
		$key = $apiKeyEl->getAttribute ( 'key' );
		if ($key == $apiKey) {
			$sharedSecret = $apiKeyEl->getAttribute ( 'sharedSecret' );
			break;
		}
	}
	$timestamp = $queryStringArr ["timestamp"];
	$signature = $queryStringArr ["signature"];
	
	$signatureString = $apiKey . $sharedSecret . $timestamp;
	$md5string = md5 ( $signatureString );
	
	$currentTime = (time () + 1) * 1000;
	if ($timestamp == "INFINITE") {
		return (($md5string === $signature)) ? 'ok' : 'error';
	}
	return (($md5string === $signature) and ($currentTime <= $timestamp)) ? 'ok' : 'error';
}

function setupHLS($url) {
	clearstatcache(true);
	$hlsURL = $url;
	
	//Check if this has any query params to process
	$queryParamPos = strpos($url, "?");
	if($queryParamPos == false) {
		return $hlsURL;
	}
	
	$queryStringArray = parseQueryString();
	
	$authResult = authenticateURL ( $queryStringArray );
	
	if ($authResult == "error") {
		header ( 'HTTP/1.0 401 Unauthorized', true, 401 );
		die ( 'Invalid URL!' );
	}
	
	$dirID = $queryStringArray ["dirID"];
	$mediaID = $queryStringArray ["mediaID"];
	
	//Check if this has correct query params
	if($dirID == "" || $mediaID == "") {
		return $hlsURL;
	}
	
	$dirPath = getDirectoryPath ($queryStringArray);
	$dirPath .= $mediaID."/hls";
	
	// check if the media has hls content  /../blob/uuid/hls
	if( ! is_dir ( $dirPath ) ) {
		return $hlsURL;
	}
	
	$hlsURL = substr($url, 0, $queryParamPos);
	
	// replace index.m3u8 with the correct UUID.m3u8
	$indexpos = strpos($hlsURL, "index.m3u8");
	$hlsURL = substr($hlsURL, 0, $indexpos);
	$hlsURL .= "/HLS/".$mediaID.".m3u8";
	
	$hlsDirPath = "/home/onair/modules/mediaroom/public/HLS";
	
	// HLS media link already setup. Do not setup again.
	if( file_exists ( $hlsDirPath."/".$mediaID ) and is_dir ( $hlsDirPath."/".$mediaID ) ) {
		return $hlsURL;
	}
	
	// Create the HLS media link.
	symlink( $dirPath, $hlsDirPath."/".$mediaID);
	//file_put_contents ( $hlsDirPath."/".$mediaID.".hlsmedia", time (), LOCK_EX );
	
	return $hlsURL;
}


function parseQueryString() {
	$raw = $_SERVER ['QUERY_STRING'];

	$arr = array ();
	$pairs = explode ( '&', $raw );

	foreach ( $pairs as $i ) {
		if (! empty ( $i )) {
			list ( $name, $value ) = explode ( '=', $i, 2 );
				
			if (isset ( $arr [$name] )) {
				if (is_array ( $arr [$name] )) {
					$arr [$name] [] = $value;
				} else {
					$arr [$name] = array (
							$arr [$name],
							$value
					);
				}
			} else {
				$arr [$name] = $value;
			}
		}
	}
	return $arr;
}

function getDirectoryPath($queryStringArr) {
	$xml = simplexml_load_file ( "/var/status/APIKeyTable.xml" );
	$dirID = $queryStringArr ["dirID"];
	$doc = new DOMDocument ();
	$doc->load ( "/etc/config/Device.xml" );
	$xpath = new DOMXPath ( $doc );
	$mountPointsList = $xpath->query ( "//MediaStore[Name='$dirID']/RootDirPath" );
	if ($mountPointsList->length > 0) {
		$mountPoint = $mountPointsList->item ( 0 );
		$path = $mountPoint->nodeValue;
		$elementName = $mountPoint->parentNode->parentNode->tagName;
		if ($elementName == 'NASEntry') {
			$nasListIndex = $mountPoint->parentNode->parentNode->getAttribute ( 'ListIndex' );
			return $path . '/' . $nasListIndex . '/blob/';
		}
		return $path . '/default/blob/';
	}
}
// Middleware creates the soft link to the actual hls folder, and passes this as url (ex: "http://<ipaddr>/mediaroom/2002.m3u8").
// Here 2002 is the folder name in mediaroom. So, this php scripts parses this and look for output.m3u8 file in this folder.
// While creating m3u8 file, we are not writing the actual URL for ts files. So, this script munges all the urls for ts files, and gives back
// to the client. If the m3u8 file not yet created, then it waits for 5 secs. And if first TS file is not yet created, then it waits for another
// 5 secs.
function doGet($url, $queryStringArray) {
    // This is the base url for hls folder. Ex: "http://<ipaddr>/mediaroom/2002".
    $url = setupHLS($url);
    clearstatcache(true);
    
    $index_file_path = '/home/onair/modules/mediaroom/public';
    
    $m3u8pos = strrpos($url, ".m3u8");
    $base_dir = substr($url, 0, $m3u8pos);
    $base_pos = strripos($url, "/", 0);
    $index_base_name = substr($url, $base_pos + 1, ($m3u8pos - $base_pos - 1));
    
    if ( file_exists($index_file_path."/HLS/".$index_base_name) ) {
    	$index_file_path .= "/HLS";
    }
    	
    $index_file_path .= '/'.$index_base_name.'/output.m3u8';
    //echo "******* indexfilepath = ".$index_file_path;
 
    $timeForIndex = 0;
    $bFoundFile = FALSE;
    
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
    } while ($timeForIndex <= 4);    

    if ($bFoundFile == FALSE) {
        header('HTTP/1.0 415 Invalid Media Path',true,415);
        echo "Invalid media path";
        syslog(LOG_NOTICE, "HLS STREAMING: Index file not found. URL: $url");
        return; 
    }

    $timeForURL = 0;
    do {
        $index_file_content = file_get_contents($index_file_path);

        // Looking for the last http url (http://IPADDRESS/.../segment-00000.ts) in the content, and looking for 
        // the correct position of "http://" and ".ts". Based on these position, extracting url for the file. 
        // Ex: "http://IPADDRESS/.../segment-10000".
        // From this we are getting base url, ie, "http://IPADDRESS/.../". Finally we are replacing this with the base url.
        $http_pos = strrpos($index_file_content, "http://");
        $ts_pos = strrpos($index_file_content, ".ts");
        if ($http_pos === false || $ts_pos === false) {
            $timeForURL += 1;
            sleep(1);
            syslog(LOG_NOTICE, "HLS STREAMING: First MPEG-TS file not found. URL: $url");
        }
        else {
            break;
        }
    } while ($timeForURL <= 4);

    //Getting the file url here. Ie, "http://IPADDRESS/.../segment-00000".
    $segment_file_url = substr($index_file_content, $http_pos, ($ts_pos - $http_pos));
    // Getting the file base here. I.e, "http://IPADDRESS/../".
    $segment_file_base = substr($segment_file_url, 0, strripos($segment_file_url, "/"));

#    header ( 'Cache-Control: max-age=1, public, must-revalidate' );
    header ( 'Content-Type:application/x-mpegURL' );

    //Replacing all the occurrences of segment base url with base url.
    //Ex: Replacing "http://IPADDRESS/.../" with "http://<ipaddr>/mediaroom/2002".
    echo str_ireplace($segment_file_base, $base_dir, $index_file_content);
}

?>
