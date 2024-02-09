#!/bin/sh
deviceloginrequest=/var/www/login.xml
startresponse=$2
setuprequestdoc=$1
streamdatadoc=/tmp/streamdata.xml
mediasourcedatadoc=/tmp/mediasourcedata.xml

CLI_STATUS=/tmp/cli_status.xml
SCRIPTS_DIR=/etc/scripts
PORT_STATUS=/tmp/port_status.xml

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates
SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache
STARTSSID=2003
AGENT_VAR=/etc/agent/db/agentvar




#get the server name/ip
if [ ! -f ${AGENT_VAR} ]; then
	devicename=`grep devicename /tmp/get_xmpp_agent.xml  | cut -f4 -d'"'`
	mcastttl=`grep multicast_ttl /tmp/get_multicast_parm.xml | cut -f4 -d'"'`
	ipaddress=`/V2O/bin/get_network_ports_status |grep IPAddress | cut -f2 -d'>' | cut -f1 -d'<'`
	fromport=`grep port_from /tmp/get_tx_control_ports_parm.xml | cut -f4 -d'"'`
	toport=`grep port_to /tmp/get_tx_control_ports_parm.xml | cut -f4 -d'"'`
	echo "devicename=${devicename}" >> ${AGENT_VAR}
	echo "mcastttl=${mcastttl}" >> ${AGENT_VAR}
	echo "ipaddress=${ipaddress}" >> ${AGENT_VAR}
	echo "fromport=${fromport}" >> ${AGENT_VAR}
	echo "toport=${toport}" >> ${AGENT_VAR}
fi
source ${AGENT_VAR}
jid=${devicename}@localhost/${devicename}
devicetype="V2D-XPi-Device"

sed -ie s/xmlns/xmlns_new/ ${setuprequestdoc}

xsltproc ${AGENT_XSL_DIR}/get_decoded_stream_data.xsl ${setuprequestdoc} | urldecoder > ${streamdatadoc}
xsltproc ${AGENT_XSL_DIR}/get_decoded_media_source_data.xsl ${setuprequestdoc} | urldecoder > ${mediasourcedatadoc}
xsltproc ${AGENT_XSL_DIR}/parse_stream_data.xsl ${streamdatadoc} > /tmp/streamvar
xsltproc ${AGENT_XSL_DIR}/get_streamnid.xsl ${setuprequestdoc} >> /tmp/streamvar
xsltproc ${AGENT_XSL_DIR}/get_streamurl.xsl ${streamdatadoc} > /tmp/streamurlvar 
source /tmp/streamvar
starttc=`date +%s`
startwc=`date +%s`
lasttc=`date +%s`
lastwc=`date +%s`
mcastttl=1
isDuplicateRequest=0


bw=`expr ${bandwidth} \* 1024`
if  [ x"${requestnid}" = "x" ]; then
	requestnid="requestNID"
fi
if [ ! -d "${DB_DIR}" ]; then
	mkdir ${DB_DIR}
fi

if [ ! -d "${DB_DIR}/$roomnid" ];then
	mkdir ${DB_DIR}/$roomnid	
fi
if [ ! -d "${DB_DIR}/$roomnid/$streamnid" ];then
	mkdir ${DB_DIR}/$roomnid/$streamnid 
else
	isDuplicateRequest=1
fi

if [ ! -f ${DB_DIR}/ssid ]; then
	echo ${STARTSSID} > ${DB_DIR}/ssid
fi

if [ x"$isDuplicateRequest" = x"0" ]; then
	echo "Duplicate startrequest StreamNID: $streamnid"
	mv -f "/tmp/streamvar" "${DB_DIR}/$roomnid/$streamnid/streamnidinfo" 
	mv -f "/tmp/streamurlvar" "${DB_DIR}/$roomnid/$streamnid/streamurl"
	mv -f $streamdatadoc "${DB_DIR}/$roomnid/$streamnid/streamdatadoc.xml"
	mv -f $mediasourcedatadoc "${DB_DIR}/$roomnid/$streamnid/mediasourcedatadoc.xml"

	if [ ! -f "${DB_DIR}/StreamInfoList.xml" ]; then
		cp ${AGENT_TEMPLATES_DIR}/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
	fi

	xsltproc -o /tmp/StreamInfoList.xml --stringparam streamnid $streamnid --stringparam streamrequestfile ${DB_DIR}/$roomnid/$streamnid/streamdatadoc.xml --stringparam mediarequestfile ${DB_DIR}/$roomnid/$streamnid/mediasourcedatadoc.xml ${AGENT_XSL_DIR}/streaminfolist.xsl ${DB_DIR}/StreamInfoList.xml

	mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml

	ssid=`cat ${DB_DIR}/ssid`
	echo  ${ssid} > ${DB_DIR}/${roomnid}/ssobj
	nextssid=`expr ${ssid} \+ 2`
	echo ${nextssid} > ${DB_DIR}/ssid


	port=${fromport}
	echo "1: port= $port"
	while(true)
		do
			if [ ${port} -gt ${toport} ]; then
				break;
			fi
			found=0;
			for i in `ls ${DB_DIR}/*/port`
			do
				tmpport=`cat $i`
				if [ x"$tmpport" = x"$port" ]; then
					found=1
					break;
				fi
			done
			if [ $found -eq 0 ]; then
				break;
			fi
			port=`expr ${port} \+ 2`
		done
	echo "2: port= $port"
	echo  ${port} > ${DB_DIR}/${roomnid}/port

#	echo url="v2d://"${ipaddress}"/port="${port}"?"${urlparams} > ${DB_DIR}/${roomnid}/${streamnid}/url
	url="v2d://"${ipaddress}"/port="${port}"?"${urlparams}

	/V2O/bin/set_tx_connection_parm  -bandwidth ${bw} \
			-burst_ratio ${burst_ratio} -burst_duration ${burst_duration} \
			-low_compression ${low_compression} \
			-high_compression ${high_compression} \
			-av_option ${av_option} -framerate_divider ${framerate_divider} \
			-min_refresh ${min_refresh} -max_refresh ${max_refresh} \
			-min_slice ${min_slice} -max_slice ${max_slice} \
			-block_threshold ${block_threshold} -color_sampling ${color_sampling}

	echo "action=add id=${ssid} operation=output streamtype=V2D dsttype=V2DDecoderPort ioport=1 authscheme=TOKEN authdata=${authdata} v2dtx=1 policycheck=0 listener=1  dstport=${port} mcastaddr=${mcastaddr} mcastttl=${mcastttl} maxbitrate=${bw}" | nc 127.0.0.1 10001
	echo "action=start id=${ssid} queueid=2001 operation=output streamtype=V2D dsttype=V2DDecoderPort" | nc 127.0.0.1 10001
fi
	#create StartResponse xml
if [ x"$isDuplicateRequest" = x"1" ]; then
	source ${DB_DIR}/${roomnid}/${streamnid}/url
fi
echo "StartResponse $roomnid $streamnid $url"
xsltproc -o ${startresponse}  \
	--stringparam roomnid $roomnid \
	--stringparam requestfile $setuprequestdoc \
	--stringparam streamnid $streamnid \
	--stringparam starttc $starttc \
	--stringparam startwc $startwc \
	--stringparam lasttc $lasttc \
	--stringparam lastwc $lastwc \
	--stringparam jid $jid \
	--stringparam tojid $dstjid \
	--stringparam url $url \
	--stringparam requestnid ${requestnid} \
	${AGENT_XSL_DIR}/startresponse.xsl  \
	${AGENT_TEMPLATES_DIR}/StartRequestResponse.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startresponse}
#logger `date` " Step 8"

