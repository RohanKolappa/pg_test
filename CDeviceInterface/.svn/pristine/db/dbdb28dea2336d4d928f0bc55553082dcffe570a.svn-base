#!/bin/sh
mediapresenceeventdoc=$1
startrequestdoc=$2
portinfo=`get_board_info |grep boardtype | cut -f4 -d'"'`

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache

stop_delete_action()
{
	type=$1
	portType=$2
	id=$3
	operation=$4
	echo "Type=$type; portType=$portType; ID=$id"
	echo "action=stop id=${id} queueid=2001 operation=${operation} streamtype=V2D ${type}=${portType}" | nc 127.0.0.1 10001
	echo "action=delete id=${id} operation=${operation} streamtype=V2D ${type}=${portType}" | nc 127.0.0.1 10001
}
stop_streaming()
{
	dirpath=$1	
	echo "Inside stop_streaming ${dirpath}"
	if [ -d ${dirpath} ]; then
		if [ -f $dirpath/streamnidinfo ]; then
			source $dirpath/streamnidinfo
		else
			roomnid=`basename $dirpath`
		fi		
		if [ -f ${DB_DIR}/$roomnid/ssobj ]; then
				ssid=`cat ${DB_DIR}/$roomnid/ssobj`
		fi		
		echo "Found SSID " ${ssid}
		stop_delete_action "dsttype" "V2DDecoderPort" $ssid "output"		
		 update_streaminfo $roomnid
		 remove_cache $roomnid
	fi

}
stop_decoding()
{
	dirpath=$1
	removecache=$2
	echo "inside stop_decoding: ${dirpath} Remove Cache: $removecache"
	if [ -d ${dirpath} ]; then
		if [ -f $dirpath/streamnidinfo ]; then
			source $dirpath/streamnidinfo
		else
			roomnid=`basename $dirpath`
		fi

		stop_delete_action "srctype" "V2DNetworkPort" "2001" "input"
		update_streaminfo $roomnid
	     if [ x"$removecache" = x"1" ]; then
			 remove_cache $roomnid
		 fi
	fi
}

send_start_request()
{
	dirpath=$1
	if [ -f $dirpath/streamnidinfo ];then
		
		streamdatadoc=`cat $dirpath/streamdatadoc.xml | urlencoder`
		mediasourcedatadoc=`cat $dirpath/mediasourcedatadoc.xml | urlencoder`

		source $dirpath/streamnidinfo
		source $dirpath/streamurl		
		echo "Update Streaminfo dirpath: $dirpath"
	    xsltproc -o /tmp/StreamInfoList.xml \
        	--stringparam streamnid $streamnid \
            --stringparam streamrequestfile $dirpath/streamdatadoc.xml \
	        --stringparam mediarequestfile $dirpath/mediasourcedatadoc.xml \
        	${AGENT_XSL_DIR}/streaminfolist.xsl ${DB_DIR}/StreamInfoList.xml
	    mv -f /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
    	echo "send start Stream dirpath:= $dirpath"
		xsltproc -o ${startrequestdoc} \
		    --stringparam jid $dstjid \
		    --stringparam tojid $tojid \
		    --stringparam streamdatadoc $streamdatadoc \
		    --stringparam mediasourcedatadoc $mediasourcedatadoc \
		    --stringparam clientdata $streamnid \
		    --stringparam streamnid $streamnid \
		    --stringparam requestnid "requestNID" \
		    ${AGENT_XSL_DIR}/startrequest.xsl \
		    ${AGENT_TEMPLATES_DIR}/SetupResponse.xml    
		sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startrequestdoc}
	fi
}

remove_cache()
{
	roomnid=$1
	if [ x"$roomnid" != "x" ]; then	
		echo "remove_cache: $roomnid"
		rm -rf ${DB_DIR}/$roomnid
	else
		echo "RoomNID is blank"
	fi

}

update_streaminfo()
{
	roomnid=$1
	echo "Updating streaminfo: $roomnid"
	if [ -f "${DB_DIR}/StreamInfoList.xml" ]; then
		xsltproc -o /tmp/StreamInfoList.xml --stringparam roomnid $roomnid ${AGENT_XSL_DIR}/removestreaminfofromlist.xsl ${DB_DIR}/StreamInfoList.xml
	mv /tmp/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
	fi 
	#remove_cache $roomnid
}

get_dirpath()
{
	inJid=$1
	dirpath=""
	echo ${inJid}
	streamfile=`grep -l -r $inJid ${DB_DIR}|grep -v "StreamInfoList.xml"`
	for file in $streamfile
	do
		dirpath=`dirname "$file"`
		break
	done
	#eval "$2=$dirpath"
}

sed -ie s/xmlns=/xmlns_new=/ ${mediapresenceeventdoc}
#parse the request and get type and value
#if type = mediaroom and value = unvailable
for i in `xsltproc ${AGENT_XSL_DIR}/handleMediaUnavailable.xsl ${mediapresenceeventdoc}`
do
	if [ x"${i}" != "x" ]; then
	dirpath=${DB_DIR}/$i
		if [ x"${portinfo}" = "xStreamDst" ]; then
			stop_decoding ${dirpath} 1
		else
			stop_streaming ${dirpath}
		fi
	fi
done
#else
#if type = entity and value = ready
for i in `xsltproc ${AGENT_XSL_DIR}/handleEntityAvailable.xsl ${mediapresenceeventdoc}`
do
	if [ x"${i}" != "x" ]; then
		if [ x"${portinfo}" = "xStreamDst" ]; then
			get_dirpath ${i}
			send_start_request ${dirpath}	
		fi	
	fi
done
#else
#if type = entity and value = unavailable
for i in `xsltproc ${AGENT_XSL_DIR}/handleEntityUnavailable.xsl ${mediapresenceeventdoc}`
do	
	if [ x"${i}" != "x" ]; then
		get_dirpath ${i} 
		if [ x"${portinfo}" = "xStreamDst" ]; then
			stop_decoding ${dirpath} 0
		else
			stop_streaming ${dirpath} 
		fi			
	fi
done

