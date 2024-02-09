<?php
handleHttpRequest ();
?>

<?php
function handleHttpRequest() {
	ob_start ();
	$url = curPageURL ();
	
	$path = parse_url ( $url, PHP_URL_PATH );
	
	$queryStringArray = parseQueryString ();
	$sourceNID = $queryStringArray ["sourceNID"];
	$imagefullpath = "";
	
	 $authResult = authenticateURL ( $queryStringArray );
	
	if ($authResult == "error") {
		header ( 'HTTP/1.0 401 Unauthorized', true, 401 );
		// cho "invalid url: <br />";
		die ( 'Invalid URL!' );
	} 
	
	if ($_SERVER ["REQUEST_METHOD"] == "GET") {
		return doGet ( $url, $path, $queryStringArray );
	} elseif ($_SERVER ["REQUEST_METHOD"] == "POST" && $path == "/mediaportal/snapshot") {
		return doPost ( $url, $path, $queryStringArray );
	} elseif ($_SERVER ["REQUEST_METHOD"] == "POST" && $path == "/mediaportal/thumbnail") {
		return doPostThumbnail ( $url, $path, $queryStringArray );
	} elseif ($_SERVER ["REQUEST_METHOD"] == "POST" && $path == "/mediaportal/file") {
		return doPostFile ( $url, $path, $queryStringArray );
	}
}
?>

<?php
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
	// cho "timestamp: ".$timestamp.'<br/>';
	// cho "signature: ".$signature.'<br/>';
	// cho "apiKey : ".$apiKey.'<br/>';
	// cho "sharedSecret : ".$sharedSecret.'<br/>';
	$md5string = md5 ( $signatureString );
	// cho "md5sharedSecret : ".$md5string.'<br/>';
	
	$currentTime = (time () + 1) * 1000;
	// cho "currentTime: ".$currentTime.'<br/>';
	if ($timestamp == "INFINITE") {
		return (($md5string === $signature)) ? 'ok' : 'error';
	}
	return (($md5string === $signature) and ($currentTime <= $timestamp)) ? 'ok' : 'error';
}
function curPageURL() {
	$pageURL = 'http';
	if ($_SERVER ["HTTPS"] == "on") {
		$pageURL .= "s";
	}
	$pageURL .= "://";
	if ($_SERVER ["SERVER_PORT"] != "80") {
		$pageURL .= $_SERVER ["SERVER_NAME"] . ":" . $_SERVER ["SERVER_PORT"] . $_SERVER ["REQUEST_URI"];
	} else {
		$pageURL .= $_SERVER ["SERVER_NAME"] . $_SERVER ["REQUEST_URI"];
	}
	
	return $pageURL;
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
		// cho 'disk table : ' .$mountPoint->parentNode->parentNode->tagName;
		$path = $mountPoint->nodeValue;
		$elementName = $mountPoint->parentNode->parentNode->tagName;
		if ($elementName == 'NASEntry') {
			$nasListIndex = $mountPoint->parentNode->parentNode->getAttribute ( 'ListIndex' );
			return $path . '/' . $nasListIndex . '/blob/';
		}
		return $path . '/default/blob/';
	}
}
function getTNFilename($queryStringArray) {
	$size = $queryStringArray ["size"];
	if ($size == "LARGE") {
		return "tn_large.jpg";
	} else if ($size == "NATIVE") {
		header ( 'HTTP/1.0 415 Unsupported Media Size', true, 415 );
		echo "Invalid size";
		return;
	}
	return "tn_small.jpg";
}
function sendTNStreamRequestToSC($queryStringArray, $action) {
	$URL = "http://localhost:10088/controller/Thumbnail?";
	$size = $queryStringArray ["size"];
	$sourceNID = $queryStringArray ["sourceNID"];
	$filename = getTNFilename ( $queryStringArray );
	
	$requestname = "Stop";
	if ($action == "START")
		$requestname = "Start";
	
	$URL = $URL . "NID=" . $sourceNID . "&size=" . $size . "&action=" . $requestname;
	$requestData = "<Request/>";
	$ch = curl_init ( $URL );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_setopt ( $ch, CURLOPT_POST, 1 );
	curl_setopt ( $ch, CURLOPT_HTTPHEADER, array (
			'Accept: text/html' 
	) );
	curl_setopt ( $ch, CURLOPT_HTTPHEADER, array (
			'Content-Type: application/xml' 
	) );
	curl_setopt ( $ch, CURLOPT_POSTFIELDS, $requestData );
	curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, 1 );
	
	$output = curl_exec ( $ch );
	
	$err = curl_errno ( $ch );
	$errmsg = curl_error ( $ch );
	$header = curl_getinfo ( $ch );
	$httpCode = curl_getinfo ( $ch, CURLINFO_HTTP_CODE );
	
	curl_close ( $ch );
	
	if ($httpCode == 202) {
		header ( 'HTTP/1.0 200 OK', true, 200 );
	} else {
		$header ['errno'] = $err;
		$header ['errmsg'] = $errmsg;
		header ( 'HTTP/1.0 ' . $httpCode, true, $httpCode );
	}
}
function getDefaultLiveTNPath($tnFilename) {
	$defaultBasePath = "/home/onair/modules/mediaportal/public/images/";
	if ($tnFilename == "tn_large.jpg") {
		return $defaultBasePath . "livethumb_large.jpg";
	}
	return $defaultBasePath . "livethumb_small.jpg";
}
function doGet($url, $path, $queryStringArray) {
	$sourceNID = $queryStringArray ["sourceNID"];
	$imagefullpath = "";
	if ($sourceNID == "") {
		
		$dirPath = getDirectoryPath ( $queryStringArray );
		// cho 'dirPath: ' . $dirPath;
		header ( 'Content-Disposition:filename=caption' );
		// eader('Content-Type:image/png');
		$mediaID = $queryStringArray ["mediaID"];
		if ($path == "/mediaportal/snapshot") {
			$ssIndexFilePath = "$dirPath/$mediaID/SSINDEX";
			$ssIndexFileContent = file_get_contents ( $ssIndexFilePath );
			$mediaFileName = substr ( $ssIndexFileContent, strrpos ( $ssIndexFileContent, ' ' ) + 1 );
			
			$doc = new DOMDocument ();
			$doc->load ( $dirPath . "/" . $mediaID . "/" . "db.xml" );
			$xpath = new DOMXPath ( $doc );
			$imageTypeList = $xpath->query ( "//MediaStoreFile/MediaImage/ImageType" );
			$imageType = '';
			$headerContent = '';
			if ($imageTypeList->length > 0) {
				$imageType = $imageTypeList->item ( 0 )->nodeValue;
			}
			if (($imageType == "PNG")) {
				$headerContent = "image/png";
			} else if (($imageType == "BMP")) {
				$headerContent = "image/bmp";
			} else if (($imageType == "GIF")) {
				$headerContent = "image/gif";
			} else if (($imageType == "TIF")) {
				$headerContent = "image/tiff";
			} else {
				$headerContent = "image/jpeg";
			}
			header ( 'Content-Type:' . $headerContent );
		} else {
			$mediaFileName = "tn/tn.jpg";
			header ( 'Content-Type:image/jpeg' );
		}
		$imagefullpath = "$dirPath/$mediaID/$mediaFileName";
	} else {
		preg_match_all ( "/^[a-zA-Z0-9.\-_]+$/", $sourceNID, $matches );
		if (max ( array_map ( 'count', $matches ) ) <= 0) {
			echo "Invalid sourceNID in URL";
			return;
		}
		
		$liveTnDirPath = "/data/tncache/live/$sourceNID";
		$tnFilename = getTNFilename ( $queryStringArray );
		
		// Choose the default image based on requested size.
		$imagefullpath = getDefaultLiveTNPath ( $tnFilename );
		
		if (! file_exists ( $liveTnDirPath ) and ! is_dir ( $liveTnDirPath )) {
			// create the source dir and send start request
			mkdir ( $liveTnDirPath, 0777, true );
			file_put_contents ( $liveTnDirPath . "/access.log", time (), LOCK_EX );
			sendTNStreamRequestToSC ( $queryStringArray, "START" );
		} else {
			
			if (! file_exists ( $liveTnDirPath . "/" . $tnFilename )) {
				$temp_image_path = $liveTnDirPath . "/tn_small.jpg";
				
				if ($tnFilename == "tn_large.jpg" && file_exists ( $temp_image_path )) {
					// if LARGE requested, but does not exist, serve SMALL if it exists.
					exec ( "cp $temp_image_path $liveTnDirPath/$tnFilename" );
				} else {
					// copy default image
					exec ( "cp $imagefullpath $temp_image_path" );
					$imagefullpath = $temp_image_path;
				}
			} else {
				// requested file exists, serve it
				$imagefullpath = $liveTnDirPath . "/" . $tnFilename;
			}
			
			if (time () - fileatime ( $imagefullpath ) > 60) {
				// if file is too old, send start request and serve default.
				$default_image_path = getDefaultLiveTNPath ( $tnFilename );
				exec ( "cp $default_image_path $imagefullpath" );
				
				sendTNStreamRequestToSC ( $queryStringArray, "START" );
			}
			file_put_contents ( $liveTnDirPath . "/access.log", time (), LOCK_EX );
			
			// remove stop status file if exists
			$stop_status_file = $liveTnDirPath . "/stop_tn_requested";
			if (file_exists ( $stop_status_file )) {
				exec ( "rm -f $stop_status_file" );
			}
		}
		header ( 'Content-Type:image/jpeg' );
	}
	
	$path_parts = pathinfo ( $imagefullpath );
	// cho $imagefullpath;
	readfile ( trim ( $imagefullpath ) );
}
function doPost($url, $path, $queryStringArray) {
	$extension = end ( explode ( ".", $_FILES ["file"] ["name"] ) );
	// echo "FileName: ".$_FILES["file"]["name"] .'<br/> ';
	// echo "Extension: ".$extension .'<br/> ';
	// echo "Type: ".$_FILES["file"]["type"] .'<br/>';
	// echo "sub string : ".substr($_FILES["file"]["type"], 0, strlen('image/')) .'<br/>';
	// echo "string cmp : ".strcmp(substr($_FILES["file"]["type"], 0, strlen('image/')),'image/') .'<br/>';
	if ((strcmp ( substr ( $_FILES ["file"] ["type"], 0, strlen ( 'image/' ) ), 'image/' ) === 0)) {
		if ($_FILES ["file"] ["error"] > 0) {
			echo "Return Code: " . $_FILES ["file"] ["error"] . "<br />";
		} else {
			// echo "Upload: " . $_FILES["file"]["name"] . "<br />";
			// echo "Type: " . $_FILES["file"]["type"] . "<br />";
			// echo "Size: " . ($_FILES["file"]["size"] / 1024) . " Kb<br />";
			// echo "Temp file: " . $_FILES["file"]["tmp_name"] . "<br />";
			
			// TBD: give permissions to /data/mediadir/import
			
			$fileName = $_FILES ["file"] ["name"];
			$uid = uniqid ( "" );
			$ext = substr ( $fileName, strrpos ( $fileName, '.' ) );
			
			$newFileName = $uid . $ext;
			$filePath = "/data/mediadir/inject/" . $newFileName;
			move_uploaded_file ( $_FILES ["file"] ["tmp_name"], $filePath );
			
			// create meta data xml
			$metadata = "<MetaData>";
			
			foreach ( $queryStringArray as $name => $value ) {
				if (($name == "apiKey") || ($name == "timestamp") || ($name == "signature") || ($name == "dirID") || ($name == "title")) {
					continue;
				}
				
				// echo "\$arr[$name] => $value.\n";
				
				$metadata .= '<Tag Name=\'' . $name . '\'>' . $value . '</Tag>';
			}
			
			$metadata .= "</MetaData>";
			
			// set IMAGE Type
			$imageType = '';
			if (($_FILES ["file"] ["type"] == "image/x-png") || ($_FILES ["file"] ["type"] == "image/png")) {
				$imageType = "PNG";
			} else if (($_FILES ["file"] ["type"] == "image/bmp")) {
				$imageType = "BMP";
			} else if (($_FILES ["file"] ["type"] == "image/gif")) {
				$imageType = "GIF";
			} else if (($_FILES ["file"] ["type"] == "image/tiff")) {
				$imageType = "TIF";
			} else {
				$imageType = "JPG";
			}
			
			$fileTitle = $queryStringArray ["title"];
			$dirId = $queryStringArray ["dirID"];
			$dirNID = "";
			$type = "MediaImage";
			$copySnapshot = "true";
			callToAddFileRequest ( $type, $imageType, $fileTitle, $dirNID, $dirId, $copySnapshot, $filePath, $metadata );
		}
	} else {
		header ( 'HTTP/1.0 415 Unsupported Media Type', true, 415 );
		echo "Invalid file";
	}
}
function doPostFile($url, $path, $queryStringArray) {
	$extension = end ( explode ( ".", $_FILES ["file"] ["name"] ) );
	
	if ($_FILES ["file"] ["error"] > 0) {
		echo "Return Code: " . $_FILES ["file"] ["error"] . "<br />";
	} else {
		
		$fileName = $_FILES ["file"] ["name"];
		$uid = uniqid ( "" );
		// $ext = substr($fileName, strrpos($fileName, '.'));
		$ext = ".mkd";
		$newFileName = $uid . $ext;
		// Move the uploaded file to temp folder
		$dirpath = "/data/mediadir/inject/";
		$filePath = $dirpath . $newFileName;
		move_uploaded_file ( $_FILES ["file"] ["tmp_name"], $filePath );
		shell_exec ( "/home/onair/TV/bin/mediaconverter -i metadata -f $filePath  > $dirpath/$uid.xml" );
		
 		$doc = new DOMDocument ();
		$doc->load ( $dirpath . "/$uid.xml" );
		$xpath = new DOMXPath ( $doc );
		$metadataEL = $xpath->query ( "//MediaStoreFile/MetaData" );
		
		$metadata = "<MetaData>";
		
		if ($metadataEL->length > 0) {
			$tags = $metadataEL->item ( 0 )->getElementsByTagName ( 'Tag' );		
			$count=0;
			foreach ( $tags as $tag ) {
				
				$name = $tag->getAttribute ( 'Name' );
				$value = $tag->nodeValue;
				$metadata .= '<Tag Name=\'' . $name . '\'>' . $value . '</Tag>'; 
			
			}
		}
		print_r($links);
		// create meta data xml
		$metadata .= "</MetaData>"; 
	
		$fileTitleEl = $xpath->query ( "//MediaStoreFile/Title" );
		if ($fileTitleEl->length > 0) {
			$fileTitle = $fileTitleEl->item ( 0 )->nodeValue;
		}
		$typeEl = $xpath->query ( "//MediaStoreFile/Type" );
		if ($typeEl->length > 0) {
			$type = $typeEl->item ( 0 )->nodeValue;
		}
		shell_exec ( "rm $dirpath/$uid.xml" );
		$dirId = $queryStringArray ["dirID"];
		$dirNID = "";
		
		// set IMAGE Type
		$imageType = 'MKD';
		$copySnapshot = "false";
		
		// cho htmlentities($xml_data, ENT_COMPAT, 'UTF-8');
		if($fileTitle!=""){
			callToAddFileRequest ( $type, $imageType, $fileTitle, $dirNID, $dirId, $copySnapshot, $filePath, $metadata );
		}
		else{
			echo "File Title is empty";
		}
	}
}
function callToAddFileRequest($type, $imageType, $fileTitle, $dirNID, $dirId, $copySnapshot, $filePath, $metadata) {
	$xml_data = '<AddFile type=\'' . $type . '\' imageType=\'' . $imageType . '\'' . ' title=\'' . $fileTitle . '\' byDirNID=\'' . $dirNID . '\'' . ' byDirID=\'' . $dirId . '\' copySnapshot=\'' . $copySnapshot . '\'' . ' filePath=\'' . $filePath . '\'>' . $metadata . '</AddFile>';
	//echo htmlentities($xml_data, ENT_COMPAT, 'UTF-8');
	$URL = "http://localhost:10080/deviceinterface/File";
	
	$ch = curl_init ( $URL );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYHOST, 0 );
	curl_setopt ( $ch, CURLOPT_SSL_VERIFYPEER, 0 );
	curl_setopt ( $ch, CURLOPT_CUSTOMREQUEST, "PUT" );
	curl_setopt ( $ch, CURLOPT_HTTPHEADER, array (
			'Content-Type: text/xml' 
	) );
	curl_setopt ( $ch, CURLOPT_POSTFIELDS, "$xml_data" );
	curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, 1 );
	
	$output = curl_exec ( $ch );
	
	$err = curl_errno ( $ch );
	$errmsg = curl_error ( $ch );
	$header = curl_getinfo ( $ch );
	$httpCode = curl_getinfo ( $ch, CURLINFO_HTTP_CODE );
	// if response failed then don't set location header
	
	curl_close ( $ch );
	
	if ($httpCode == 201) {
		$xml = new SimpleXMLElement ( $output );
		$url = $xml ['url'];
		$fileNID = $xml ['NID'];
		header ( "Location: " . $url );
		header ( "X-IPVS-mediaNID: " . $fileNID );
		header ( 'HTTP/1.0 201 Created', true, 201 );
		// cho '<br/>'. $url;
		// cho '<br/>'.$fileNID;
		$output = $url;
	} else {
		$header ['errno'] = $err;
		$header ['errmsg'] = $errmsg;
		header ( 'HTTP/1.0 ' . $httpCode, true, $httpCode );
	}
	
	echo $output;
}
function doPostThumbnail($url, $path, $queryStringArray) {
	if (! (strcmp ( substr ( $_FILES ["file"] ["type"], 0, strlen ( 'image/' ) ), 'image/' ) === 0)) {
		header ( 'HTTP/1.0 415 Unsupported Media Type', true, 415 );
		echo "Invalid file";
		return;
	}
	if ($_FILES ["file"] ["error"] > 0) {
		echo "Return Code: " . $_FILES ["file"] ["error"] . "<br />";
		return;
	}
	
	$tnFilename = getTNFilename ( $queryStringArray );
	if ($tnFilename == "") {
		return;
	}
	
	$sourceNID = $queryStringArray ["sourceNID"];
	if ($sourceNID == "") {
		echo "Invalid URL";
		return;
	}
	
	preg_match_all ( "/^[a-zA-Z0-9.\-_]+$/", $sourceNID, $matches );
	if (max ( array_map ( 'count', $matches ) ) <= 0) {
		echo "Invalid sourceNID in URL";
		return;
	}
	
	$filePath = "/data/tncache/live/" . $sourceNID;
	
	if (! file_exists ( $filePath ) and ! is_dir ( $filePath )) {
		mkdir ( $filePath, 0777, true );
		file_put_contents ( $filePath . "/access.log", time (), LOCK_EX );
	}
	
	$lastaccess_time = file_get_contents ( $filePath . "/access.log" );
	
	// if(($timestamp = strtotime($lastaccess_time)) !== false) {
	// echo "Invalid access time";
	// return;
	// }
	
	// If last access if older than expiry threshold (in seconds)
	if (time () - $lastaccess_time > 300) {
		// request SC to stop posting TNs and write the status file
		if (! file_exists ( $filePath . "/stop_tn_requested" )) {
			sendTNStreamRequestToSC ( $queryStringArray, "STOP" );
			file_put_contents ( $filePath . "/stop_tn_requested", time (), LOCK_EX );
		}
	} else {
		move_uploaded_file ( $_FILES ["file"] ["tmp_name"], $filePath . "/" . $tnFilename );
	}
}

?>
