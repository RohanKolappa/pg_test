<?php
$CONTROLLER_PORT=10001;
function send_request($command) {
	$service_port=10001;
	$address="127.0.0.1";
	$socket = socket_create(AF_INET, SOCK_STREAM,0);
	if ($socket < 0) {
	}
	if ($socket < 0) {
   		echo "socket_create() : " . socket_strerror($socket) . "\n";
	} else {
   		echo "OK.\n";
	}
	$CONTROLLER_PORT=10001;
   	echo "Connecting $service_port $CONTROLLER_PORT.\n";
	$result = socket_connect($socket, $address, $service_port);
	if ($result < 0) {
   		echo "socket_connect() : ($result) " . socket_strerror($result) . "\n";
	} else {
   		echo "OK.\n";
		$cmdlen=strlen($command);
		$result=socket_write($socket,$command, $cmdlen);
		if ($result < 0) {
   			echo "socket_write() : ($result) " . socket_strerror($result) . "\n";
		}
		$out = socket_read($socket, 1024);
		echo "$out";
		socket_close($socket);
	}
}
	echo "Enter input\n";
	$fstdin = fopen('php://stdin', 'r');
	$input="";
	while  ($input != "quit") {
		$cmd="";
		$info = fscanf($fstdin,"%s");
		$input = $info[0];
		switch($input) {
			case "addrx":
				$cmd= "action=add id=6546 operation=input srctype=V2DNetworkPort streamtype=V2D srcip=192.168.1.137 srcbitrate=1000000 srcport=6060 srcmulticast=0\n";
				break;
			case "startrx":
				$cmd="action=start id=6546 queueid=1000 operation=input srctype=V2DNetworkPort\n";
				break;
			case "stoprx":
				$cmd="action=stop id=6546 operation=input srctype=V2DNetworkPort\n";
				break;
			case "deleterx":
				$cmd="action=delete id=6546 operation=input srctype=V2DNetworkPort\n";
				break;
			case "addwr":
				$cmd= "action=add id=2227 operation=output dsttype=DMSRecorderPort streamtype=V2D dstname=R3\n";
				break;
			case "startwr":
				$cmd="action=start id=2227 queueid=1000 operation=output dsttype=DMSRecorderPort\n";
				break;
			case "stopwr":
				$cmd="action=stop id=2227 operation=output dsttype=DMSRecorderPort\n";
				break;
			case "deletewr":
				$cmd="action=delete id=2227 operation=output dsttype=DMSRecorderPort\n";
				break;
			case "addrdr":
				$cmd= "action=add id=6546 operation=input srctype=V2DMediaPort streamtype=V2D srcname=R2\n";
				break;
			case "startrdr":
				$cmd="action=start id=6546 queueid=1000 operation=input srctype=V2DMediaPort\n";
				break;
			case "stoprdr":
				$cmd="action=stop id=6546 operation=input srctype=V2DMediaPort\n";
				break;
			case "deleterdr":
				$cmd="action=delete id=6546 operation=input srctype=V2DMediaPort\n";
				break;
			case "addtx":
				$cmd= "action=add id=7777 operation=output dsttype=V2DDecoderPort streamtype=V2D dstip=192.168.1.1 dstport=6060 dstmulticast=0\n";
				break;
			case "starttx":
				$cmd="action=start id=7777 queueid=1000 operation=output dsttype=V2DDecoderPort\n";
				break;
			case "stoptx":
				$cmd="action=stop id=7777 queueid=1000 operation=output dsttype=V2DDecoderPort\n";
				break;
			case "deletetx":
				$cmd="action=delete id=7777 operation=output dsttype=V2DDecoderPort\n";
				break;
			case "stattx":
				$cmd="action=status id=7777 operation=output dsttype=V2DDecoderPort\n";
				break;
		}
		if ($cmd) {
			send_request($cmd);
			echo $cmd;
		}
	}
?>
