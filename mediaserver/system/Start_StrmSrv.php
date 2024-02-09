<?php

//
// Globals
//
$ROOT = "/home/onair";
$V2O_HOMEDIR = "/V2O";
$g_V2OServerConf	= $V2O_HOMEDIR . "/config/serverconf.txt";
$g_RFBConf	= $V2O_HOMEDIR . "/config/txrfbconf_0.txt";
$PHP            	= $ROOT . "/apache/bin/php";
$ipadd = "127.0.0.1";
$V2DDecoderPortID = 1000;
$V2DEncoderPortID = 1001;

$g_StandAloneOperation = $V2O_HOMEDIR . "/config/standaloneoperation.txt";

// This below condition kicks off streamingserver in stand alone mode else proceed further.

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
	print("Usage: php Start_StrmSrv.php\n");
	print("No args starts both v2dtx and fpgarx\n");
	print("If args specified updates either of them\n");
}

function parseArguments() {
	global $argv;

       $res = strncasecmp($argv[1],"startall",strlen("startall"));
       if($res == 0){
        StartAll();
        return TRUE;
       }

       $res = strncasecmp($argv[1],"updatev2dtx",strlen("updatev2dtx"));
       if($res == 0){
        UpdateV2dTx();
        return TRUE;
       }

       $res = strncasecmp($argv[1],"updatefpgarx",strlen("updatefpgarx"));
       if($res == 0){
        UpdateFpgaRx();
        return TRUE;
       }

       $res = strncasecmp($argv[1],"updaterfb",strlen("updaterfb"));
       if($res == 0){
        UpdateRFB();
        return TRUE;
       }


       $res = strncasecmp($argv[1],"disconnectall",strlen("disconnectall"));
       if($res == 0){
        V2dTxDisconnectAll();
        return TRUE;
       }

       $res = strncasecmp($argv[1],"stopall",strlen("stopall"));
       if($res == 0){
        StopAll();
        return TRUE;
       }
       else
        print " Input is $argv[1] --> Unknown\n";


        return TRUE;

}

function UpdateFpgaRx(){

            global $g_V2OServerConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;
 $updatescript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=update id=$V2DEncoderPortID srctype=V2DEncoderPort\" 10001\n";
           system($updatescript);

}

function UpdateRFB(){

            global $g_RFBConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;
            
            $rfb_enable = `grep ENABLE $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/ENABLE//'`;
            $rfb_ipaddress = `grep IPADDRESS $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/IPADDRESS//'`;
            $rfb_port = `grep PORT $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/PORT//'`;
            $rfb_password = `grep PASSWORD $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/PASSWORD//'`;
            $rfb_enable = strncasecmp($rfb_enable,"yes",strlen("yes"));
            if($rfb_enable == 0)
                $rfb_enable = 1;
            else
                $rfb_enable = 0;

           $rfb_enable = str_replace("\n","",$rfb_enable);
           $rfb_ipaddress = str_replace("\n","",$rfb_ipaddress);
           $rfb_port = str_replace("\n","",$rfb_port);
           $rfb_password = str_replace("\n","",$rfb_password);

            $updatescript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=update id=$V2DEncoderPortID srctype=V2DEncoderPort rfbenable=$rfb_enable rfbip=$rfb_ipaddress rfbport=$rfb_port rfbpasswd=$rfb_password\" 10001\n";
           system($updatescript);

}

function UpdateV2dTx() {

            global $g_V2OServerConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;

            $updatescript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=update id=$V2DDecoderPortID dsttype=V2DDecoderPort\" 10001\n";
           system($updatescript);

}

function V2dTxDisconnectAll(){

            global $g_V2OServerConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;

            $updatescript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=update id=$V2DDecoderPortID dsttype=V2DDecoderPort disconnectall=1\" 10001\n";
           system($updatescript);


}

function StopAll() {

            global $g_V2OServerConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;

         
            $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=stop id=$V2DEncoderPortID queueid=$V2DEncoderPortID operation=input srctype=V2DEncoderPort\" 10001\n";
           system($stopscript);
           $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=stop id=$V2DDecoderPortID operation=output queueid=$V2DEncoderPortID dsttype=V2DDecoderPort\" 10001\n";
           system($stopscript);
            $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=delete id=$V2DEncoderPortID queueid=$V2DEncoderPortID operation=input srctype=V2DEncoderPort\" 10001\n";
           system($stopscript);
           $stopscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=delete id=$V2DDecoderPortID operation=output queueid=$V2DEncoderPortID dsttype=V2DDecoderPort\" 10001\n";
           system($stopscript);
}

function StartAll() {

            global $g_V2OServerConf,$g_RFBConf,$PHP,$ROOT,$ipadd,$V2DDecoderPortID,$V2DEncoderPortID,$V2O_HOMEDIR;

 
            $control_port = `grep CONTROL_PORT $g_V2OServerConf | sed -e's/[[:space:]]*//g' | sed -e's/CONTROL_PORT//'`;
            $multicast_address = `grep MULTICAST_ADDRESS $g_V2OServerConf | sed -e's/[[:space:]]*//g' | sed -e's/MULTICAST_ADDRESS//'`;
            $multicast_ttl = `grep MULTICAST_TTL $g_V2OServerConf | sed -e's/[[:space:]]*//g' | sed -e's/MULTICAST_TTL//'`;
            $rfb_enable = `grep ENABLE $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/ENABLE//'`;
            $rfb_ipaddress = `grep IPADDRESS $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/IPADDRESS//'`;
            $rfb_port = `grep PORT $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/PORT//'`;
            $rfb_password = `grep PASSWORD $g_RFBConf | sed -e's/[[:space:]]*//g' | sed -e's/PASSWORD//'`;

            $rfb_enable = strncasecmp($rfb_enable,"yes",strlen("yes"));
            if($rfb_enable == 0)
                $rfb_enable = 1;
            else
                $rfb_enable = 0;


           // To replace newline characters int he string
           $control_port = str_replace("\n","",$control_port);
           $multicast_address = str_replace("\n","",$multicast_address);
           $multicast_ttl = str_replace("\n","",$multicast_ttl);
           $rfb_enable = str_replace("\n","",$rfb_enable);
           $rfb_ipaddress = str_replace("\n","",$rfb_ipaddress);
           $rfb_port = str_replace("\n","",$rfb_port);
           $rfb_password = str_replace("\n","",$rfb_password);



           $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=add id=$V2DEncoderPortID operation=input srctype=V2DEncoderPort rfbenable=$rfb_enable rfbip=$rfb_ipaddress rfbport=$rfb_port rfbpasswd=$rfb_password\" 10001\n";
           system($startscript);

           $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=start id=$V2DEncoderPortID queueid=$V2DEncoderPortID operation=input srctype=V2DEncoderPort\" 10001\n";
           system($startscript);

           $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=add id=$V2DDecoderPortID operation=output dsttype=V2DDecoderPort streamtype=V2D dstip=$ipadd dstport=$control_port mcastaddr=$multicast_address mcastttl=$multicast_ttl  v2dtx=1 listener=1\" 10001\n";
           system($startscript);

           $startscript = "$PHP /etc/scripts/streamingserver.php $ipadd \"action=start id=$V2DDecoderPortID operation=output queueid=$V2DEncoderPortID dsttype=V2DDecoderPort\" 10001\n";
           system($startscript);

}

//
// Main script starts from here
//

?>
