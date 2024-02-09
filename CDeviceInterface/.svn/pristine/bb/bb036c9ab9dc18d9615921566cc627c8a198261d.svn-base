#!/bin/sh
source /etc/agent/scripts/common.rc
startcompletedoc=$2
startresponsedoc=$1

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache

OLD="STREAMNID"
if [ ! -f $startresponsedoc ]; then
	exit
fi
#get the server name/ip
#get the server name/ip
portinfo=`get_board_info |grep boardtype | cut -f4 -d'"'`
videoport=`get_rx_av_ports_parm |grep port_from | cut -f4 -d'"'`
audioport=`expr ${videoport} + 1`

sed -ie s/xmlns=/xmlns_new=/ ${startresponsedoc}
xsltproc ${AGENT_XSL_DIR}/parse_start_response_data.xsl ${startresponsedoc} > /tmp/streamvar
source /tmp/streamvar
mv -f "/tmp/streamvar" "${DB_DIR}/$roomnid/$streamnid/url"
xsltproc -o ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml ${AGENT_XSL_DIR}/get_streamstatus_data.xsl ${startresponsedoc}

source ${DB_DIR}/$roomnid/$streamnid/streamurl
if [ x"$portinfo" = x"StreamSrc" ] ; then
	starttc=`date +%s`
	lasttc=`date +%s`
	lastwc=`date +%s`
	xsltproc -o ${startcompletedoc} --stringparam jid $jid --stringparam tojid $toagentjid --stringparam streamnid $streamnid --stringparam starttc $starttc --stringparam lasttc $lasttc --stringparam lastwc $lastwc --stringparam status "200" ${AGENT_XSL_DIR}/sourceevent.xsl ${AGENT_TEMPLATES_DIR}/SourceEvent.xml
		        sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startcompletedoc}
else
	#create	networkport for rx
	echo "creating network port for rx: url: $url"
	sscmd=`echo ${url} | sed -e 's/\?/ /g' | sed -e 's/\/port/ srcport/'\
		| sed -e 's/bandwidth/srcbitrate/'\
        | sed -e 's/true/1/' | sed -e 's/false/0/' | sed -e 's/\/\//srcip=/'\
        | sed -e 's/v2d://' | sed -e 's/ismulticast/srcmulticast/'\
		| sed -e 's/Video/1/' | sed -e 's/Both/3/' | sed -e 's/Audio/2/'`
	#echo "action=stop id=2001 queueid=2001 operation=input srctype=V2DNetworkPort"  | nc 127.0.0.1 10001
	#echo "action=delete id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort" | nc 127.0.0.1 10001

	echo "action=add id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort ${sscmd} videoport=${videoport} audioport=${audioport}" | nc 127.0.0.1 10001
	echo "action=start id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort" | nc 127.0.0.1 10001
fi
if [ -f "${DB_DIR}/StreamInfoList.xml" ]; then
	sed -ie "s/$OLD/$streamnid/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
	xsltproc -o /tmp/StreamInfoList.xml --stringparam streamnid $streamnid --stringparam roomnid $roomnid --stringparam statusfile ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml ${AGENT_XSL_DIR}/updatestreamstatus.xsl ${DB_DIR}/StreamInfoList.xml
	mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
	sed -ie "s/$streamnid/$OLD/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
fi
