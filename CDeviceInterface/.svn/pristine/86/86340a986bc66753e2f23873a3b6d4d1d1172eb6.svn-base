#!/bin/sh
source /etc/agent/scripts/common.rc
getconfigresponsedoc=$2
getconfigrequestdoc=$1

configfile=/etc/config/Device.xml
tmpconfigfile=/tmp/Config.xml

SCRIPTS_DIR=/etc/scripts

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

sed -ie s/xmlns=/xmlns_new=/ ${getconfigrequestdoc}
sed -e s/xmlns=/xmlns_new=/ ${configfile} > ${tmpconfigfile}

xsltproc -o ${getconfigresponsedoc} \
	--stringparam requestfile ${getconfigrequestdoc} \
	--stringparam configfile ${tmpconfigfile} \
	${AGENT_XSL_DIR}/getconfighandler.xsl  \
	${AGENT_TEMPLATES_DIR}/GetDeviceConfigResponse.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${getconfigresponsedoc}

