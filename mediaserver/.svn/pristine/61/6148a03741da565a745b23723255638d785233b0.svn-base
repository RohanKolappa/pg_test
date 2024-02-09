#!/usr/bin/php
<?php

$cmdarg=$_SERVER["argv"];
$SERVER=$cmdarg[1];
$SSCOMMAND=$cmdarg[2];
$SERVERPORT=10001;

if (!($cmdarg[1] && $cmdarg[2])) {
	echo "Usage ./streamingserver.php <ipaddress> <command> [port] \n";
	exit;
}
if ($cmdarg[3]) {
        $SERVERPORT=$cmdarg[3];
}

function send_request( $address, $service_port, $command) {
	$socket = socket_create(AF_INET, SOCK_STREAM,0);
	if ($socket < 0) {
	}
	if ($socket < 0) {
   		echo "socket_create() : " . socket_strerror($socket) . "\n";
	} else {
   		//echo "OK.\n";
	}
	$CONTROLLER_PORT=10001;
   	//echo "Connecting $service_port $CONTROLLER_PORT.\n";
	$result = socket_connect($socket, $address, $service_port);
	if ($result < 0) {
   		echo "socket_connect() : ($result) " . socket_strerror($result) . "\n";
	} else {
   		//echo "OK.\n";
		$cmdlen=strlen($command);
		$result=socket_write($socket,$command, $cmdlen);
		if ($result < 0) {
   			echo "socket_write() : ($result) " . socket_strerror($result) . "\n";
		}

		$out = "";
		while (1) {
                    $str= socket_read($socket, 1024);
                    if ($str == FALSE || $str == "")
                        break;
                    $out .= $str;
                    if (!(strstr($str, "\n") === false)) {
                        break;
                    }
		}
		echo "$out";
		socket_close($socket);
	}

}
	printf ("$SSCOMMAND\n");
	send_request($SERVER, $SERVERPORT, $SSCOMMAND);

?>
