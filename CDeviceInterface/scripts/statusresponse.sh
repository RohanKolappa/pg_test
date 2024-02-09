#!/bin/sh
source /etc/agent/scripts/common.rc
statusresponse=$2
statusrequestdoc=$1
statusinformation=/tmp/Device.xml
SCRIPTS_DIR=/etc/scripts

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

#get the server name/ip
devicetype="V2D-XPi-Device"
sed -ie s/xmlns=/xmlns_new=/ ${statusrequestdoc}
# run all the status requests and concatenate the xmls
for i in cpu_status memory_status
do
	get_${i} > /tmp/${i}.xml
done

xsltproc -o /var/www/out/status.xml \
			 --stringparam cpustatus /tmp/cpu_status.xml \
			 --stringparam memorystatus /tmp/memory_status.xml \
			 ${AGENT_XSL_DIR}/update_status.xsl ${statusinformation}

/bin/mv /var/www/out/status.xml $statusinformation
xsltproc -o ${statusresponse}  \
	--stringparam requestfile $statusrequestdoc \
	--stringparam statusinfo $statusinformation \
        --stringparam jid $jid \
	${AGENT_XSL_DIR}/statusresponse.xsl  \
	${AGENT_TEMPLATES_DIR}/GetDeviceStatusResponse.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${statusresponse}

