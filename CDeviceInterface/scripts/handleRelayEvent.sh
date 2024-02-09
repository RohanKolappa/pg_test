#!/bin/sh
source /etc/agent/scripts/common.rc
startcompletedoc=$2
relayeventdoc=$1

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts
DB_DIR=/etc/agent/db/streamcache


sed -ie s/xmlns=/xmlns_new=/ ${relayeventdoc}
xsltproc ${AGENT_XSL_DIR}/parse_relay_event_data.xsl ${relayeventdoc} > /tmp/relaydata
source /tmp/relaydata

streamfile=`grep -l -r $mediasourcenid ${DB_DIR}|grep -v "StreamInfoList.xml"`
relaydirpath=`dirname "$streamfile"`

echo "RelayDirPath = $relaydirpath"

mv /tmp/relaydata $relaydirpath

source ${relaydirpath}/relaydata

echo "DirPath = $relaydirpath"

if [ -f $relaydirpath/streamstatus.xml ];
then
	source $relaydirpath/streamnidinfo
	#change the state the relay data of the streamstatus xml
        xsltproc -o /tmp/streamstatus.xml --stringparam relayroomnid $relayroomnid --stringparam relaystreamnid $relaystreamnid ${AGENT_XSL_DIR}/change_state.xsl $relaydirpath/streamstatus.xml
	mv /tmp/streamstatus.xml $relaydirpath/streamstatus.xml
	xsltproc -o ${startcompletedoc} \
	    --stringparam jid $jid \
            --stringparam streamnid $streamnid \
            --stringparam roomnid $roomnid \
            --stringparam requestfile $relaydirpath/streamstatus.xml \
	    ${AGENT_XSL_DIR}/startcomplete.xsl \
	    ${AGENT_TEMPLATES_DIR}/StartComplete.xml    
	sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${startcompletedoc}
else
	echo "Start Response not yet recieved"
fi
