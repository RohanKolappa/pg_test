<?php

//
// Globals
//
$ROOT = "/home/onair";
$V2O_HOMEDIR = "/V2O";
$g_V2OClientConf	= $V2O_HOMEDIR . "/config/clientconf.txt";
$PHP            	= $ROOT . "/apache/bin/php";
$ipadd = "127.0.0.1";
$V2DDecoderPortID = 2000;
$V2DEncoderPortID = 2001;

$g_StandAloneOperation = $V2O_HOMEDIR . "/config/standaloneoperation.txt";

// The below condition kicksoff streaming server in stand alone mode else proceed further. 

$standalone = `grep STANDALONE $g_StandAloneOperation | sed -e's/[[:space:]]*//g' | sed -e's/STANDALONE//'`;
$standalone = str_replace("\n","",$standalone);
$res = strncasecmp($standalone,"Yes",strlen("Yes"));
if($res != 0){
    return 1;
}


if(FALSE == parseArguments()) {
	printUsage();
	return 1;
}

//
// Functions
//

function printUsage() {
	print("Usage: php Start_StrmSrvRx.php\n");
	print("No args starts both v2drx and fpgatx\n");
	print("If args specified updates either of them\n");
}

function parseArguments() {
	global $argv;

    $res = strncasecmp($argv[1],"startall",strlen("startall"));
    if($res == 0){
        StartAll();
        return TRUE;
    }

    $res = strncasecmp($argv[1],"stopall",strlen("stopall"));
    if($res == 0){
        StopAll();
        return TRUE;
    }
    $res = strncasecmp($argv[1],"reconnect",strlen("reconnect"));
    if($res == 0){
        ReConnect();
        return TRUE;
    }
    else
        print("Input is $argv[1] --> Unknown\n");


    return TRUE;

}

function StopNetwork() {

    global $PHP,$ipadd,$V2DEncoderPortID,$V2DDecoderPortID;

         
    $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=stop id=$V2DEncoderPortID queueid=$V2DDecoderPortID operation=input srctype=V2DNetworkPort\" 10001\n";
    system($stopscript);

    $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=delete id=$V2DEncoderPortID queueid=$V2DDecoderPortID operation=input srctype=V2DNetworkPort\" 10001\n";
    system($stopscript);

}
function StopFpga() {

    global $PHP,$ipadd,$V2DDecoderPortID;

         
    $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=stop id=$V2DDecoderPortID queueid=$V2DDecoderPortID operation=output dsttype=V2DHardWarePort\" 10001\n";
    system($stopscript);

    $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=delete id=$V2DDecoderPortID queueid=$V2DDecoderPortID operation=output dsttype=V2DHardWarePort\" 10001\n";
    system($stopscript);

}

function StopAll() {

    StopNetwork();
    StopFpga();

}

function StartNetwork() {

    global $g_V2OClientConf,$PHP,$ipadd,$V2DEncoderPortID,$V2DDecoderPortID;

 
    $av_port = `grep AV_PORT $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/AV_PORT//'`;
    $server_port = `grep SERVER_PORT $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/SERVER_PORT//'`;
    $server_address = `grep SERVER_ADDRESS $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/SERVER_ADDRESS//'`;
    $avoption = `grep AVOPTION $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/AVOPTION//'`;
    $max_bandwidth = `grep MAX_BANDWIDTH $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/MAX_BANDWIDTH//'`;
    $sample_size = `grep SAMPLESIZE $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/SAMPLESIZE//'`;
    $mono_stereo = `grep MONOSTEREO $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/MONOSTEREO//'`;
    $multicast = `grep MULTICAST $g_V2OClientConf | sed -e's/[[:space:]]*//g' | sed -e's/MULTICAST//'`;

    // To replace new line characters in the string
    $av_port = str_replace("\n","",$av_port);
    $server_port = str_replace("\n","",$server_port);
    $server_address = str_replace("\n","",$server_address);
    $avoption = str_replace("\n","",$avoption);
    $max_bandwidth = str_replace("\n","",$max_bandwidth);
    $sample_size = str_replace("\n","",$sample_size);
    $mono_stereo = str_replace("\n","",$mono_stereo);
    $multicast = str_replace("\n","",$multicast);
    $video_port = $av_port;
    $audio_port = $av_port + 1;

    // convert avoption to enum that StrmSrv expects
    $audio_video_option = 1;
    if (strncasecmp($avoption,"audio",strlen("audio")) == 0) {
        $audio_video_option = 2;
    };
    if (strncasecmp($avoption,"both",strlen("both")) == 0) {
        $audio_video_option = 3;
    };
    $max_bandwidth = $max_bandwidth * 8; // convert to bits per second

    $samplesize = 0;
    if (strncasecmp($sample_size,"16Bit",strlen("16Bit")) == 0) {
        $samplesize = 0;
    };
    if (strncasecmp($sample_size,"8Bit",strlen("8Bit")) == 0) {
        $samplesize = 1;
    };
    $monostereo = 0;
    if (strncasecmp($mono_stereo,"Stereo",strlen("Stereo")) == 0) {
        $monostereo = 0;
    };
    if (strncasecmp($mono_stereo,"Mono",strlen("Mono")) == 0) {
        $monostereo = 1;
    };
    $audio_type = 0;
    $audio_type = 2 * $samplesize + $monostereo;
    $srcmulticast = 0;
    if (strncasecmp($multicast,"yes",strlen("yes")) == 0) {
        $srcmulticast = 1;
    };

    if($server_address != "0.0.0.0") {

        $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=add id=$V2DEncoderPortID operation=input srctype=V2DNetworkPort srcip=$server_address srcmulticast=$srcmulticast srcport=$server_port srcbitrate=$max_bandwidth avoption=$audio_video_option videoport=$video_port audioport=$audio_port audiotype=$audio_type\" 10001\n";
        system($startscript);

        $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=start id=$V2DEncoderPortID queueid=$V2DDecoderPortID operation=input srctype=V2DNetworkPort\" 10001\n";
        system($startscript);
    }
}

function StartFpga() {

    global $PHP,$ipadd,$V2DDecoderPortID;

    $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=add id=$V2DDecoderPortID operation=output dsttype=V2DHardWarePort srcport=0 \" 10001\n";
    system($startscript);

    $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=start id=$V2DDecoderPortID queueid=$V2DDecoderPortID operation=output dsttype=V2DHardWarePort srcport=0\" 10001\n";
    system($startscript);

}

function StartAll() {
    StartNetwork();
    StartFpga();
}

function ReConnect(){
    StopNetwork();
    StartNetwork();
}

?>
