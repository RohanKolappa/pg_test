<?php
	if (!file_exists("/data/demos")) {
		print "Please enable demo mode to access this page";
		exit;
	}
?>


<?php

	$SHAREPATH = "/tmp/share";
	$MAXFILES = 5; 

	$query_string = $_SERVER['QUERY_STRING'];
	
	$params = explode(";", $query_string);
	// print "$params[0]\n$params[1]\n$params[2]\n";

	if(substr($_SERVER['QUERY_STRING'], 0, strlen("share=")) == "share=") {	
		$shareid = substr($_SERVER['QUERY_STRING'], strlen("share="));
		$handle = fopen($SHAREPATH . "/" . $shareid, "r") or die("Unable to find share=".$shareid);;
		while( ($line = fgets($handle)) != false)	
		  print trim($line);
	}
	else {
		$htmlStr = substr($params[0], strlen("html="));
		$htmlStr = base64_decode($htmlStr);
	
		if(count($params) == 3) {	// This is for generating the shareURL
			$sourceListStr = substr($params[1], strlen("sourcelist="));
			$urlListStr = substr($params[2], strlen("urllist="));
			
			// print "ulist=$urlListStr\nslist=$sourceListStr\nhtml=$htmlStr=html\n";	
			
			$urlList = explode(",", $urlListStr);
			$sourceList = explode(",", $sourceListStr);
			for($i = 0; $i < count($sourceList) -1; $i++) {
				$id = base64_decode($sourceList[$i]);
				$url = $urlList[$i];
				$url = base64_decode($url);	
				$htmlStr = str_replace($id, $url,  $htmlStr);		
			}
			
			// Create the share directory if required
			if(!is_dir($SHAREPATH)) {
				mkdir($SHAREPATH);
			}
			
			// print URL to the share file
			$t = time();
			$shareid = $t."-".md5($htmlstr).".html";
			$handle = fopen($SHAREPATH . "/" . $shareid, "w") or die("Unable to write share=".$shareid);;
			fwrite($handle, $htmlStr);

			// Age and remove the older share files if exceeds max files - TBD
			$files = scandir($SHAREPATH, 1);
			$x = count($files);
			while($x > $MAXFILES + 2) {
				$x--;
				if(($files[$x] == ".")  || ($files[$x] == ".."))
					continue;
				$filename = $SHAREPATH."/".$files[$x];
				// print "unlink ".$filename."<br>";
				unlink($filename);
			}
			
			$htmlStr = '{"shareid":"'.$shareid.'"}';
		}
		print $htmlStr;
	}
?>
