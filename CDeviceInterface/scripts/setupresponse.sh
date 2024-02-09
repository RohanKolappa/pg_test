#!/bin/sh
source /etc/agent/scripts/common.rc
startrequestdoc=$2
setuprequestdoc=$1
streamdatadoc=/tmp/streamdata.xml
mediasourcedatadoc=/tmp/mediasourcedata.xml

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates
OLD="STREAMNID"
SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache
streaminfodoc={$DB_DIR}/StreamInfo.xml
#get the server name/ip
#get the server name/ip


sed -ie s/xmlns=/xmlns_new=/ ${setuprequestdoc}

xsltproc ${AGENT_XSL_DIR}/get_decoded_stream_data.xsl ${setuprequestdoc} | urldecoder > ${streamdatadoc}
xsltproc ${AGENT_XSL_DIR}/get_decoded_media_source_data.xsl ${setuprequestdoc} | urldecoder > ${mediasourcedatadoc}
xsltproc ${AGENT_XSL_DIR}/parse_stream_data.xsl ${streamdatadoc} > /tmp/streamvar           
xsltproc ${AGENT_XSL_DIR}/get_streamnid.xsl ${setuprequestdoc} >> /tmp/streamvar           
xsltproc ${AGENT_XSL_DIR}/get_streamurl.xsl ${streamdatadoc} > /tmp/streamurlvar           
source /tmp/streamurlvar                                                                    
source /tmp/streamvar                                                                       

if [ ! -d "${DB_DIR}" ]; then
	mkdir ${DB_DIR}
fi

if [ ! -d "${DB_DIR}/$roomnid" ];then
	mkdir ${DB_DIR}/$roomnid
	mkdir ${DB_DIR}/$roomnid/$streamnid 
fi
mv -f "/tmp/streamvar" "${DB_DIR}/$roomnid/$streamnid/streamnidinfo" 
mv -f "/tmp/streamurlvar" "${DB_DIR}/$roomnid/$streamnid/streamurl"
mv -f $streamdatadoc "${DB_DIR}/$roomnid/$streamnid/streamdatadoc.xml"
mv -f $mediasourcedatadoc "${DB_DIR}/$roomnid/$streamnid/mediasourcedatadoc.xml"

if [ ! -f "${DB_DIR}/StreamInfoList.xml" ]; then
	cp ${AGENT_TEMPLATES_DIR}/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
fi

xsltproc -o /tmp/StreamInfoList.xml --stringparam streamnid $streamnid --stringparam streamrequestfile ${DB_DIR}/$roomnid/$streamnid/streamdatadoc.xml --stringparam mediarequestfile ${DB_DIR}/$roomnid/$streamnid/mediasourcedatadoc.xml ${AGENT_XSL_DIR}/streaminfolist.xsl ${DB_DIR}/StreamInfoList.xml

mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
if [ x"$streamurl" = "x" ] ; then                                                
	if [ x"$requestnid" = "x" ] ; then                                               
        	requestnid="requestNID"                                                  
	fi                                
	# get streamURL if blank then    
	#create Startrequest              
	xsltproc -o ${startrequestdoc} \
	    --stringparam jid $jid \
	    --stringparam tojid $tojid \
	    --stringparam requestfile $setuprequestdoc \
	    --stringparam streamnid $streamnid \
	    --stringparam requestnid $requestnid \
	    ${AGENT_XSL_DIR}/setupresponse.xsl \
	    ${AGENT_TEMPLATES_DIR}/SetupResponse.xml    
	sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startrequestdoc}
else
	echo "Direct connection with autostart"

	videoport=`get_rx_av_ports_parm |grep port_from | cut -f4 -d'"'`
	audioport=`expr ${videoport} + 1`

	starttc=`date +%s`
	startwc=`date +%s`
	lasttc=`date +%s`
	lastwc=`date +%s`
	sscmd=`echo ${streamurl} | sed -e 's/\?/ /g' | sed -e 's/\/port/ srcport/'\
		| sed -e 's/bandwidth/srcbitrate/'\
        | sed -e 's/true/1/' | sed -e 's/false/0/' | sed -e 's/\/\//srcip=/'\
        | sed -e 's/v2d://' | sed -e 's/ismulticast/srcmulticast/'\
		| sed -e 's/Video/1/' | sed -e 's/Both/3/' | sed -e 's/Audio/2/'`
	echo "action=add id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort ${sscmd} videoport=${videoport} audioport=${audioport}" | nc 127.0.0.1 10001
	echo "action=start id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DNetworkPort" | nc 127.0.0.1 10001
	xsltproc -o ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml --stringparam roomnid $roomnid --stringparam streamnid $streamnid --stringparam starttc $starttc --stringparam startwc $startwc --stringparam lasttc $lasttc --stringparam lastwc $lastwc --stringparam url $streamurl  --stringparam state "8"  ${AGENT_XSL_DIR}/createstreamstatus.xsl ${AGENT_TEMPLATES_DIR}/StreamStatusEvent.xml

	 xsltproc -o ${startrequestdoc} --stringparam jid $jid --stringparam roomnid $roomnid --stringparam streamnid $streamnid --stringparam statusfile ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml ${AGENT_XSL_DIR}/startcompletedirect.xsl ${AGENT_TEMPLATES_DIR}/StartCompleteDirect.xml	
	sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startrequestdoc}
	if [ -f "${DB_DIR}/StreamInfoList.xml" ]; then
		sed -ie "s/$OLD/$streamnid/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
		xsltproc -o /tmp/StreamInfoList.xml --stringparam streamnid $streamnid --stringparam roomnid $roomnid --stringparam statusfile ${DB_DIR}/$roomnid/$streamnid/streamstatus.xml ${AGENT_XSL_DIR}/updatestreamstatus.xsl ${DB_DIR}/StreamInfoList.xml
		mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
		sed -ie "s/$streamnid/$OLD/" ${AGENT_XSL_DIR}/updatestreamstatus.xsl
	fi
fi    
