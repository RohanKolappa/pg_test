<?php
$cmdarg=$_SERVER["argv"];
$SERVER=$cmdarg[1];
$SERVERPORT=10001;
$ITER=$cmdarg[2];
function waitforsignal($f)
{
	$input="";
	//while($input != "go") {
		$info = fscanf($f,"%s");
		$input = $info[0];
	//}
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
		$out = socket_read($socket, 1024);
		echo "$out";
		socket_close($socket);
	}
}
$fstdin = fopen('php://stdin', 'r');
$txid=50;
$txport=7070;
$rxid=1142556000;
$svrport=6060;
$vport=6060;
$aport=6061;	
$qid=1000;
for ($i=0; $i < $ITER; $i++) {

	$cmd = "action=stop id=${txid} operation=output dsttype=V2DDecoderPort\n";
	printf("$cmd");
	send_request($SERVER, $SERVERPORT, $cmd);

	$cmd = "action=delete id=${txid} operation=output dsttype=V2DDecoderPort\n";
	printf("$cmd");
	send_request($SERVER, $SERVERPORT, $cmd);

	$cmd = "action=stop id=${rxid} operation=input srctype=V2DNetworkPort\n";
	printf("$cmd");
	send_request($SERVER, $SERVERPORT, $cmd);

	$cmd = "action=delete id=${rxid} operation=input srctype=V2DNetworkPort\n";
	printf("$cmd");
	send_request($SERVER, $SERVERPORT, $cmd);

	$txid=$txid+2;
	$txport=$txport+2;
	$rxid=$rxid+2;
	$svrport=$svrport+2;
	$vport=$vport+2;
	$aport=$aport+2;
	$qid=$qid+2;
}
