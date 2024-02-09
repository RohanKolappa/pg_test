<?php
	if(PHP_SAPI == "cli") {
		$token = $argv[1];
		$media = $argv[2];
	}
	else {
		/*
		foreach($_SERVER as $key_name => $key_value) {
			print $key_name."=".$key_value."<br>";
		}
		*/
		$token = $_COOKIE["token"];
		$media = $_GET["media"];
		if(!isset($token)) {
			// $token = time();
			$token = "miyerpc";
			setcookie("token", $token, time() + (86400 * 7));
		}
	}

?>

<script type="text/javascript">
function handleLaunch(clientUrl, id, retryUrl)
{
	alert("Launch clienUrl=" + clientUrl + " id=" + id + " retryUrl=" + retryUrl);
	window.location = clientUrl;
}

function handleRetry(url)
{
	alert("Retry url=" + url);
	window.location = url;
}

function handleClose()
{
	alert("Close");
	window.close();
}
</script>

<?php
ini_set('display_errors', 1);
ini_set('log_errors', 1);
// error_reporting(E_ALL);
class Client {
	private $id = null;
	private $tokendir = "/tmp/token";
	private $state = "new";
	private $error = null;
	private $filename = null;
	private $jid = null;
	private $response = null;

	function init($token) {
		$this->id = $token;
		$filename = $this->getFilename($token);
		if(!file_exists($filename)) {
			$this->createFile($filename);
			return;
		}
		$this->state = "wait";
		if(filesize($filename) == 0) {
			return;
		}
		$this->jid = file_get_contents($filename) or $this->error="could not read token file";
		$this->jid = trim($this->jid);
		$this->state = "ready";
	}

	function setMedia($media) {
		$request_uri = $_SERVER["REQUEST_URI"];
		$response = "<script type='text/javascript'>\n";
		$message = "";	
		if($this->state == "new") {
			$clientUrl = "client.php";
			$windowId = $this->id;
			$response = $response."handleLaunch('".$clientUrl."','".$windowId."','".$request_uri."')\n"; 
			// $response = $response."setTimeout('handleRetry(\'".$request_uri."\')', 5000)\n";
			// $message = "Launching client";
		}
		else if($this->state == "wait") {
			$response = $response."setTimeout('handleRetry(\'".$request_uri."\')', 5000)\n";
			$message = "Waiting for client";
		}
		else {
			$this->queueMedia($media);
			$response = $response."setTimeout('handleClose()', 5000)\n";
			$message = "Client Launched";
		}
		$this->response = $response."</script>\n".$message."\n";
	}

	function getFilename($token) {
		return $this->tokendir."/".$token;
	}

	function getState() {
		return $this->state;
	}

	function getError() {
		return $this->error;
	}

	function getResponse() {
		return $this->response;
	}


	function createFile($filename) {
		$handle = fopen($filename, 'w') or $this->error = "cannot create token file";
		fclose($handle);
	}

	function queueMedia($media) {
		$queue = $this->tokendir."/queue";
		$handle = fopen($queue, 'a') or $this->error = "cannot open queue";
		$portTitle = $this->id; // TBD Get portTitle from JID
		$sessionId = $this->id;
		$request = "<iq type='set'><query xmlns='com.ipvs.session'><Playback sessionId='".$sessionId."' ><GrpFile id='' fileNID='".$media."'/><DstPort portTitle='".$portTitle."'/></Playback></query></iq>";
		$this->response = $this->response.$request;
		fwrite($handle, $request) or $this->error = "cannot write to queue";
		fclose($handle);
	}

}


	$client = new Client();
	$client->init($token);
	$client->setMedia($media);
	if(PHP_SAPI == "cli") {
		print "state=".$client->getState()." error=".$client->getError()." response=".$client->getResponse()."\n";
	}
	else {
		print $client->getResponse();
	}
?>

<!--
test.php
<a href='launch.php?media=123'>current</a>
<a href='launch.php?media=123' target="_blank">newtarget</a>
<a href='launch.php?media=123' onclick='window.open(this.href); return false;'>newwindow</a>

EMRUI -> launch UI -> client UI : 

1. Normally there will be 3 pages
2. The launch UI will close once the client is ready
3. If there is no client then the client UI is loaded in the same page as the launch UI

-->

