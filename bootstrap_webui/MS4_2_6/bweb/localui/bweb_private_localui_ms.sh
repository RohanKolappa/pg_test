#!/bin/ash
# barco web interface functions
# (C) 2009-2011 Peter Korsgaard <peter.korsgaard@barco.com>

# location of html/menu templates
BWEB_TEMPLATE_DIR="/home/onair/htdocs/public/templates/localui"

# parpoolfs mount point
BWEB_PPFS_DIR=/mnt/fuse/parpool

# bweb configuration directory
BWEB_CONF_DIR=/usr/share/bweb

# default htpasswd file to reset to
BWEB_DEFAULT_PASSWD=/mnt/rom-etc/htpasswd

# session id generator
BWEB_GENID=$BWEB_CONF_DIR/bweb-genid

BWEB_DEVICENAME=ms

# Parse menu description and output HTML/javascript menu.
#
# Menu description has form:
# minauth level url text (where url == '-' means no link)
#
# url can be a shell expression. Remember to quote it if it contains spaces.
# line will be skipped if url expands to the empty string.
#
_bweb_menu() {
	echo "<ul id='menu'>"
	local prevlevel=1
	local line=1
	local port_filter="Decoding"
	case $boardtype in
		" Encoder ")
			port_filter="Decoding"
			;;
		" Decoder ")
			port_filter="Encoding"
			;;
		*)
			port_filter="Encoding"
			;;
	esac
	grep -v '^#' $BWEB_TEMPLATE_DIR/$BWEB_DEVICENAME/menu.txt | grep -v ${port_filter} |
	while read row;
	do
	# split according to quotes
	eval set -- $row
	minauth=$1
	level=$2
	eval url=$3
	shift 3
	text="$*"

	# skip lines with empty urls or requiring higher access
	[ -n "$url" -a $bweb_auth_level -ge $minauth ] || continue
	while [ $prevlevel -gt $level ];
	do
		echo "</ul></li>"
		prevlevel=$(( $prevlevel - 1 ))
	done

	# barco style strangely enough clips at level=2
	[ $level -gt 2 ] && level=2

	if [ "$url" == "-" ];
	then
		name=$line$(echo "text"|sed 's/[^a-zA-Z0-9]/_/g');
		echo "<li class='menulevel${level}close' id='menu${name}'>"
		echo "<a href=\"javascript:toggle('menu${name}');\">$text</a>"
		echo "<ul id='menu${name}1' class='hide'>"
	else
		css=''
		# /cgi-bin/index.cgi should be treated as / for busybox httpd
		cururl=${SCRIPT_NAME%%cgi-bin/index.cgi}
		[ "$url" == "$cururl" ] && css=" style='font-weight:bold'"
		echo "<li class='menulevel${level}link'><a href='$url'$css>$text</a></li>"
	fi
	prevlevel=$level
	line=$(( $line + 1 ));
	done

	while [ $prevlevel -gt 1 ];
	do
	echo "</ul></li>"
	prevlevel=$(( $prevlevel - 1 ))
	done

	echo "</ul>"
	echo "<script type='text/javascript' src='/js/barco-menu.js'></script>"
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_menu" >&2
}

_bweb_signature() {
	_bweb_getupdatedconfig "version"
	_bweb_getupdatedconfig "adminop"
	_bweb_getupdatedconfig "board_info"

	swver=`grep swver /tmp/get_version.xml| cut -f4 -d'"'`
	boardtype=`grep boardtype /tmp/get_board_info.xml| cut -f4 -d'"'`
	standalone=`grep standalone /tmp/get_adminop.xml| cut -f4 -d'"'`

	hostname=`hostname`
	if [ x${boardtype} = "xStreamSrc" ]; then
		boardtype=" Encoder "
	else
		boardtype=" Decoder "
	fi
	if [ x${standalone} = "xYes" ]; then
		standalone=" Standalone "
	else
		standalone=" Managed "
	fi
	BWEB_SIGNATURE=`echo MS $hostname `
	[ "$_DEBUG" == "on" ] && echo "EXIT _bweb_signature" >&2
}

bweb_getboardtype() {
	boardtype=`/V2O/bin/get_board_info | grep boardtype| cut -f4 -d'"'`
	if [ x${boardtype} = "xStreamSrc" ]; then
		export boardtype=ENC
	else
		export boardtype=DEC
	fi
	[ "$_DEBUG" == "on" ] && echo "EXIT bweb_getboardtype" >&2
}

_bweb_cli_special_case() {

	i=$1
	cmd=$2

	local x=`echo $cmd | tr '\-' '\n' | tr ' ' ';'`
	local finalcmd=""

	for j in $x
	do
		j=`echo $j | tr ';' ' '`
		cmd="-$j"

		case "$j" in 
			*enablediscovery*)
				cmdcli="/V2O/bin/set_xmpp_server"
				bweb_execute_cli "$i" "$cmdcli" "$cmd"

			;;
			*enableuserregistry*)
				cmdcli="/V2O/bin/set_xmpp_server"
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
			;;
			*enablentp*)
				cmdcli="/V2O/bin/set_ntp_server"
				cmd=`echo $cmd | sed "s/enablentp/enable/g"`
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
				cmdcli="/V2O/bin/set_ntp"
				cmd="-enable=\"No\""
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
				;;
			*ntpip*)
				cmdcli="/V2O/bin/set_ntp"
				cmd=`echo $cmd | sed "s/ntpip/ipaddress/g"`
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
				cmd="-enable=\"Yes\""
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
				cmdcli="/V2O/bin/set_ntp_server"
				cmd="-enable=\"No\""
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
			;;
			*serverip*)
				cmdcli="/V2O/bin/set_xmpp_agent"
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
			;;
			*autodetect*)
				cmdcli="/V2O/bin/set_xmpp_agent"
				cmd=`echo $cmd | sed "s/autodetect/enable/g"`
				isAutoDetect=`echo $cmd | grep "Yes"`
				
				if [ -n "$isAutoDetect" ]; then
					cmd=`echo $cmd | sed 's#Yes#No#g'`
				else
					cmd=`echo $cmd | sed 's#No#Yes#g'`
				fi
				bweb_execute_cli "$i" "$cmdcli" "$cmd"
			;;
			*deviceType*)
				cmdcli="/V2O/bin/set_xmpp_server"
				cmd=`echo $cmd | sed 's#deviceType#enable#g'`
				isCombo=`echo $cmd | grep "MS/DMS"`
				
				if [ -n "$isCombo" ]; then
					cmd=`echo $cmd | sed 's#MS/DMS#Yes#g'`
					bweb_execute_cli "$i" "$cmdcli" "$cmd"

					cmdcli="/V2O/bin/set_xmpp_agent"
					cmd="-enable=\"Yes\""
					bweb_execute_cli "$i" "$cmdcli" "$cmd"
					cmd="-serverip=\"127.0.0.1\""
					bweb_execute_cli "$i" "$cmdcli" "$cmd"
				else
					cmd=`echo $cmd | sed 's#DMS#No#g'`
					bweb_execute_cli "$i" "$cmdcli" "$cmd"
				fi
			;;
		esac
	done
}

_bweb_cli_special_case_ha() {
	i=$1
	cmd=$2

	local x=`echo $cmd | tr '\-' '\n' | tr ' ' ';'`
	local cmdcli="/V2O/bin/set_ha"
	cmd=""
#	local finalcmd=""

	for j in $x
	do
		j=`echo $j | tr ';' ' '`
		
		case "$j" in 
			*enabledns*)
				LOCALDOMAIN=`/V2O/bin/get_dns_client | grep 'name="localdomain"' | cut -f4 -d'"'`
				enabledns=`echo $j | grep "Yes"`
				if [ -n "$enabledns" ]; then
 					otherCmdCli="/V2O/bin/set_xmpp_agent"
					otherCmd="-serverip ms.$LOCALDOMAIN"
				fi

				#do nothing. $j only needs to be cleared up from cmd.
			;;
			*enableha*)
				temp=`echo $j | sed "s/enableha/enable/g"`
				isEnabled=`echo $temp | grep "Yes"`
				
				if [ ! -n "$isEnabled" ]; then
					otherCmdCli="/V2O/bin/set_xmpp_agent"
					otherCmd="-serverip 127.0.0.1"
				fi
				cmd=$cmd" -$temp"
			;;
			*secondary*)
				temp=`echo $j | sed "s/secondary/primary/g"`
				enableassecondary=`echo $temp | grep "Yes"`
				
				if [ -n "$enableassecondary" ]; then
					temp=`echo $temp | sed 's#Yes#No#g'`
				else
					temp=`echo $temp | sed 's#No#Yes#g'`
				fi

				cmd=$cmd" -$temp"
			;;
			*virtualip2*)
				cmd=$cmd" -$j"
			;;
			*virtualip*)
				otherCmdCli="/V2O/bin/set_xmpp_agent"
				otherCmd=`echo $j | sed "s/\<virtualip\>/-serverip/g"`

				cmd=$cmd" -$j"
			;;
			*)
				cmd=$cmd" -$j"
			;;			
		esac
	done

	bweb_execute_cli "$i" "$cmdcli" "$cmd"
	
	if [ -n "$otherCmdCli" ]; then
		bweb_execute_cli "XMPP Agent" "$otherCmdCli" "$otherCmd"
		otherCmdCli=""
	fi
}