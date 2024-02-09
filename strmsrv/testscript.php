<?php
$CONTROLLER_PORT=10001;
$dms1port=10001;
$dms1ip="127.0.0.1";
$dms2port=10001;
$dms2ip="192.168.1.211";
$br=2000000;
$txip="192.168.1.190";
$badtxip="192.168.1.138";
$relaydms="192.168.1.210";


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
function setupsyncstarttx ($a, $b, $c)
{
	global $br;
	$cmd= "action=syncstart id=1001 operation=input srctype=V2DNetworkPort streamtype=V2D idlist=$c\n";
	send_request ($a, $b, $cmd);
}
function syncstoptx ($a, $b, $c)
{
	global $br;
	$cmd= "action=syncstop id=7000 operation=input srctype=V2DNetworkPort streamtype=V2D idlist=$c\n";
	send_request ($a, $b, $cmd);
}

function setupbadtx ($a, $b, $c)
{
	global $br;
	$cmd= "action=add id=1001 operation=input srctype=V2DNetworkPort streamtype=V2D srcip=$c srcbitrate=$br srcport=6060 srcmulticast=0\n";
	send_request ($a, $b, $cmd);
}
function setuptx ($a, $b, $c)
{
	global $br;
	$cmd= "action=add id=1000 operation=input srctype=V2DNetworkPort streamtype=V2D srcip=$c srcbitrate=$br srcport=6060 srcmulticast=0\n";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=1000 queueid=1000 operation=input srctype=V2DNetworkPort\n";
	send_request ($a, $b, $cmd);
}
function stoptx ($a, $b, $c)
{
	$cmd="action=stop id=1000 operation=input srctype=V2DNetworkPort\n";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=1000 operation=input srctype=V2DNetworkPort\n";
	send_request ($a, $b, $cmd);
}
function setuprecord ($a,$b, $c)
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$cmd= "action=add id=4000 operation=output dsttype=DMSRecorderPort streamtype=V2D dstname=$c\n";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=4000 queueid=1000 operation=output dsttype=DMSRecorderPort\n";
	send_request ($a, $b, $cmd);
}
function stoprecord ($a,$b, $c)
{
	$cmd="action=stop id=4000 queueid=1000 operation=output dsttype=DMSRecorderPort\n";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=4000 queueid=1000 operation=output dsttype=DMSRecorderPort\n";
	send_request ($a, $b, $cmd);
}
function setupsyncmedia ($a,$b, $c, $d)
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$cmd= "action=add id=5000 operation=input srctype=V2DMediaPort streamtype=V2D srcname=$c tc=$d\n";
	send_request ($a, $b, $cmd);
	$cmd= "action=add id=5001 operation=input srctype=V2DMediaPort streamtype=V2D srcname=$c tc=$d\n";
	send_request ($a, $b, $cmd);
}
function startsyncmedia ($a,$b, $c)
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$cmd="action=syncstart idlist=5000,5001 queueid=1000,1001 operation=input srctype=V2DMediaPort\n";
	send_request ($a, $b, $cmd);
}
function stopsyncmedia ($a,$b, $c, $d)
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$cmd="action=syncstop idlist=5000,5001 queueid=1000,1001 operation=input srctype=V2DMediaPort\n";
	send_request ($a, $b, $cmd);

	$cmd= "action=delete id=5000 operation=input srctype=V2DMediaPort streamtype=V2D \n";
	send_request ($a, $b, $cmd);
	$cmd= "action=delete id=5001 operation=input srctype=V2DMediaPort streamtype=V2D \n";
	send_request ($a, $b, $cmd);
}
function setupplaymedia ($a,$b, $c, $d)
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$cmd= "action=add id=5000 operation=input srctype=V2DMediaPort streamtype=V2D srcname=$c tc=$d\n";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=5000 queueid=1000 operation=input srctype=V2DMediaPort\n";
	send_request ($a, $b, $cmd);
}
function stopplaymedia ($a,$b, $c)
{
	$cmd="action=stop id=5000 queueid=1000 operation=input srctype=V2DMediaPort\n";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=5000 queueid=1000 operation=input srctype=V2DMediaPort\n";
	send_request ($a, $b, $cmd);
}
function setuprx ($a, $b, $c)
{
	global $br;
	$cmd= "action=add id=2000 operation=output dsttype=V2DDecoderPort streamtype=V2D dstip=192.168.1.1 dstport=6060 dstmulticast=0\n";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=2000 queueid=1000 operation=output dsttype=V2DDecoderPort\n";
	send_request ($a, $b, $cmd);
}
function stoprx ($a, $b, $c)
{
	$cmd="action=stop id=2000 operation=output dsttype=V2DDecoderPort\n";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=2000 operation=output dsttype=V2DDecoderPort\n";
	send_request ($a, $b, $cmd);
}

function setuprecording ($a, $b, $a1, $c, $d)
{
	global $br, $relaydms;
	$cmd= "action=add id=20" . "$d" . " operation=output dsttype=V2DDecoderPort streamtype=V2D dstip=192.168.1.1 dstport=60". "$d". "0 dstmulticast=0\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=20". "$d". " queueid=1000 operation=output dsttype=V2DDecoderPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);

	$cmd= "action=add id=30". "$d". "  operation=input srctype=V2DNetworkPort streamtype=V2D srcip=$relaydms srcbitrate=$br srcport=60". "$d". "0 srcmulticast=0\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);
	$cmd="action=start id=30". "$d". " queueid=30". "$d" . " operation=input srctype=V2DNetworkPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

	$cmd= "action=add id=40". "$d". " operation=output dsttype=DMSRecorderPort streamtype=V2D dstname=CLIP". "$d" . "\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);
	$cmd="action=start id=40" . "$d". " queueid=30" . "$d" . " operation=output dsttype=DMSRecorderPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

}
function stoprecording ($a, $b, $a1, $c, $d)
{
	$cmd="action=stop id=40". "$d" . " operation=output dsttype=DMSRecorderPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

	$cmd="action=delete id=40". "$d" . " operation=output dsttype=DMSRecorderPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

	$cmd="action=stop id=30". "$d". " queueid=30". "$d" . " operation=input srctype=V2DNetworkPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

	$cmd="action=delete id=30". "$d". " queueid=30". "$d" . " operation=input srctype=V2DNetworkPort\n";
	echo " $a1 $c $cmd";
	send_request ($a1, $c, $cmd);

	$cmd="action=stop id=20". "$d". " queueid=1000 operation=output dsttype=V2DDecoderPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=20". "$d". " queueid=1000 operation=output dsttype=V2DDecoderPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
}

function setupplayback ($a, $b, $c)
{
	$cmd= "action=add id=50" . "$c" . " operation=output dsttype=V2DDecoderPort streamtype=V2D dstip=192.168.1.1 dstport=70". "$c". "0 dstmulticast=0\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
	$cmd="action=start id=50". "$c". " queueid=60". "$c" ." operation=output dsttype=V2DDecoderPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);

	$cmd= "action=add id=60". "$c". " operation=input srctype=V2DMediaPort streamtype=V2D srcname=CLIP". "$c" . "\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);

	$cmd="action=start id=60". "$c". " queueid=60". "$c". " operation=input srctype=V2DMediaPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
}
function stopplayback ($a, $b, $c)
{
	$cmd="action=stop id=50". "$c". "  operation=output dsttype=V2DDecoderPort\n";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=50". "$c". "  operation=output dsttype=V2DDecoderPort\n";
	send_request ($a, $b, $cmd);

	$cmd="action=stop id=60". "$c". " operation=input srctype=V2DMediaPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
	$cmd="action=delete id=60". "$c". " operation=input srctype=V2DMediaPort\n";
	echo " $a $b $cmd";
	send_request ($a, $b, $cmd);
}
function waitforsignal($f)
{
	$input="";
	while($input != "go") {
		$info = fscanf($f,"%s");
		$input = $info[0];
	}
}
function do_syncstart_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms,$badtxip;
	$ids="2,3245,4";
	$fstdin = fopen('php://stdin', 'r');
	echo "Sending SYNC START DMS-I \n";
	setupsyncstarttx($dms1ip, $dms1port,$ids);
	waitforsignal($fstdin);
	exit(0);
}
function do_badtx_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms,$badtxip;
	$fstdin = fopen('php://stdin', 'r');
	echo "Connecting the BADTX to the DMS-I $badtxip\n";
	setupbadtx($dms1ip, $dms1port,$badtxip);
	waitforsignal($fstdin);
	exit(0);
}
function do_tx_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$rxip="192.168.1.1";
	$fstdin = fopen('php://stdin', 'r');
	echo "Connecting the TX to the DMS-I\n";
	setuptx($dms1ip, $dms1port,$txip);
	waitforsignal($fstdin);
	echo "Disconnect the TX -> DMS-I\n";
	//stoptx($dms1ip, $dms1port, $txip);
	stoptx($dms1ip, $dms1port, 1000);
}
function do_rxtx_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$rxip="192.168.1.1";
	$fstdin = fopen('php://stdin', 'r');
	echo "Connecting the RX to the DMS-I\n";
	setuprx($dms1ip, $dms1port, $rxip);
	waitforsignal($fstdin);
	echo "Connecting the TX to the DMS-I\n";
	setuptx($dms1ip, $dms1port,$txip);
	waitforsignal($fstdin);
	echo "Disconnect the RX -> DMS-I\n";
	stoprx($dms1ip, $dms1port, $rxip);
	waitforsignal($fstdin);
	echo "Disconnect the TX -> DMS-I\n";
	stoptx($dms1ip, $dms1port, $txip);
}
function do_playbackrx_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$mediaid="clip";
	$timecode=1142890065796;
	$rxip="192.168.1.1";
	$fstdin = fopen('php://stdin', 'r');
	echo "Connecting the RX to the DMS-I\n";
	setuprx($dms1ip, $dms1port, $rxip);
	waitforsignal($fstdin);
	echo "Connecting the TX to the DMS-I\n";
	setupplaymedia($dms1ip, $dms1port,$mediaid,$timecode);
	waitforsignal($fstdin);
	echo "Disconnect the RX -> DMS-I\n";
	stoprx($dms1ip, $dms1port, $rxip);
	waitforsignal($fstdin);
	echo "Disconnect the TX -> DMS-I\n";
	stopplaymedia($dms1ip, $dms1port, $mediaid);
}
function do_syncmedia_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$mediaid="clip";
	$timecode=1142890065796;
	$rxip="192.168.1.1";
	$fstdin = fopen('php://stdin', 'r');
	echo "Setup the synchmedia\n";
	setupsyncmedia($dms1ip, $dms1port,$mediaid,$timecode);
	waitforsignal($fstdin);
	echo "Start Sync media\n";
	startsyncmedia($dms1ip, $dms1port, $rxip);
	waitforsignal($fstdin);
	echo "Stop sync media\n";
	stopsyncmedia($dms1ip, $dms1port, $mediaid);
}
function do_recordtx_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$mediaid="clip";
	$fstdin = fopen('php://stdin', 'r');
	echo "Setup reord\n";
	setuprecord($dms1ip, $dms1port,$mediaid);
	waitforsignal($fstdin);
	echo "Connecting the TX to the DMS-I\n";
	setuptx($dms1ip, $dms1port,$txip);
	waitforsignal($fstdin);
	echo "Stop record\n";
	stoprecord($dms1ip, $dms1port,$mediaid);
	waitforsignal($fstdin);
	echo "Disconnect the TX -> DMS-I\n";
	stoptx($dms1ip, $dms1port, $txip);
}

function do_test()
{
	global $dms1port, $dms1ip, $dms2port, $dms2ip, $br, $txip, $relaydms;
	$fstdin = fopen('php://stdin', 'r');

	$max=0;
	while(($max  < 1)  || ($max > 9)){
		echo "Enter the maximum Tx's\n";
		$info = fscanf($fstdin,"%s");
		$max = $info[0];
	}

	echo "Connecting the TX to the DMS(190)\n";
	setuptx($dms1ip, $dms1port, $txip);
	waitforsignal($fstdin);
	
	for ($i=0; $i < $max; $i++) {
		setuprecording ($dms1ip, $dms1port, $dms2ip, $dms2port, $i);
		echo "Recording ($i) from DMS(190) to DMS(175)\n";
		sleep(1);
	}
	waitforsignal($fstdin);
	
	for ($i=0; $i < $max; $i++) {
		stoprecording ($dms1ip, $dms1port, $dms2ip, $dms2port, $i);
		echo "Stop Recording ($i) from DMS(190) to DMS(175)\n";
		sleep(1);
	}
	stoptx($dms1ip, $dms1port, $txip);
	echo "Disconnecting the TX from the DMS(190)\n";

	echo "Now test Playback of recorded content\n";
	waitforsignal($fstdin);
	for ($i=0; $i < $max; $i++) {
		setupplayback($dms2ip, $dms2port, $i);
		echo "Playback Recording ($i) from DMS(175) to V2DRx\n";
		sleep(1);
	}
	waitforsignal($fstdin);
	for ($i=0; $i < $max; $i++) {
		stopplayback($dms2ip, $dms2port, $i);
		echo "Stopping Playback ($i) from DMS(175) to V2DRx\n";
		sleep(1);
	}
}
	//do_test();
	//do_badtx_test();
	//do_tx_test();
	do_rxtx_test();
	//do_syncrxtx_test();
	//do_recordtx_test();
	//do_syncstart_test();
	//do_playbackrx_test();
	//do_syncmedia_test();
?>
