#!/bin/sh
source /etc/agent/scripts/common.rc
streamstatuseventdoc=$2
sourceeventdoc=$1

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache

OLD="STREAMNID"

#get the server name/ip
#get the server name/ip
portinfo=`get_board_info |grep boardtype | cut -f4 -d'"'`
videoport=`get_rx_av_ports_parm |grep port_from | cut -f4 -d'"'`
audioport=`expr ${videoport} + 1`

sed -ie s/xmlns=/xmlns_new=/ ${sourceeventdoc}
xsltproc ${AGENT_XSL_DIR}/parse_source_event_data.xsl ${sourceeventdoc} > /tmp/streamvar
source /tmp/streamvar
path=`find $DB_DIR -print0 -type f| grep $streamnid/streamdatadoc.xml`
path=`dirname "$path"` 

#xsltproc -o $path/streamstatus.xml ${AGENT_XSL_DIR}/get_sourceevent_data.xsl ${sourceeventdoc}

#sscmd=`echo ${url} | sed -e 's/\?/ /g' | sed -e 's/\/port/ srcport/'\
#		| sed -e 's/bandwidth/srcbitrate/'\
#        | sed -e 's/true/1/' | sed -e 's/false/0/' | sed -e 's/\/\//srcip=/'\
#        | sed -e 's/v2d://' | sed -e 's/ismulticast/srcmulticast/'\
#		| sed -e 's/Video/1/' | sed -e 's/Both/3/' | sed -e 's/Audio/2/'`

#echo "action=add id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort ${sscmd} videoport=${videoport} audioport=${audioport}" #| nc 127.0.0.1 10001
#echo "action=start id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort" | nc 127.0.0.1 10001
source $path/streamurl
source $path/url

#if dest, generate streamstatus event when sourcevent is recieved
# if event code is 111 or 116 and evenet level is 200
#	set state as 8
#	if event code is 111
#		update the Last element from status to Last element from incoming xml
#	else
#		Update the lastEventData from status to lastEventData from incoming xml	
#else
#	set state as 203
#	remove last lastElementData
#	add Exception element to the status

if [ -f $path/relaydata ]; then
	source $path/relaydata
else
	echo "relay event not recieved"
	if [ x"$relayroom" = "x" ] ; then
	        relayroomnid='""'
	else
		relayroomnid=$relayroom
	fi
	if [ x"$relaystream" = "x" ] ; then
	        relaystreamnid='""'
	else
		relaystreamnid=$relaystream
	fi
fi
if [ x"$eventlevel" = x"200" ]; then
	if [ x"$eventcode" = x"111" ]; then
	echo "Source event code =111"	
        xsltproc -o /tmp/streamstatus.xml --stringparam state "8" --stringparam relayroomnid $relayroomnid --stringparam relaystreamnid $relaystreamnid -stringparam requestfile $sourceeventdoc ${AGENT_XSL_DIR}/change_state_streaming.xsl $path/streamstatus.xml
	else
		if [ x"$eventcode" = x"116" ]; then
			echo "Source event code =116"
			xsltproc -o /tmp/streamstatus.xml --stringparam state "8" --stringparam relayroomnid $relayroomnid --stringparam relaystreamnid $relaystreamnid -stringparam requestfile $sourceeventdoc ${AGENT_XSL_DIR}/change_state_source_connected.xsl $path/streamstatus.xml
		fi
	fi
mv /tmp/streamstatus.xml $path/streamstatus.xml
else #source event with error
	echo "Source event with error : $eventlevel"
	xsltproc -o /tmp/streamstatus.xml --stringparam state "203" --stringparam relayroomnid $relayroomnid --stringparam relaystreamnid $relaystreamnid -stringparam requestfile $sourceeventdoc ${AGENT_XSL_DIR}/change_state_error.xsl $path/streamstatus.xml
mv /tmp/streamstatus.xml $path/streamstatus.xml
fi

source $path/streamnidinfo
xsltproc -o ${streamstatuseventdoc} --stringparam jid $jid --stringparam streamnid $streamnid --stringparam roomnid $roomnid --stringparam requestfile $path/streamstatus.xml ${AGENT_XSL_DIR}/startcomplete.xsl ${AGENT_TEMPLATES_DIR}/StartComplete.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${streamstatuseventdoc}
	
if [ -f "${DB_DIR}/StreamInfoList.xml" ]; then
	sed -ie "s/$OLD/$streamnid/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
	xsltproc -o /tmp/StreamInfoList.xml --stringparam streamnid $streamnid --stringparam roomnid $roomnid --stringparam statusfile ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml ${AGENT_XSL_DIR}/updatestreamstatus.xsl ${DB_DIR}/StreamInfoList.xml
	mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
	sed -ie "s/$streamnid/$OLD/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
fi

