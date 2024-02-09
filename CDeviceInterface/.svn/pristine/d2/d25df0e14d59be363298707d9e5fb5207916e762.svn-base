#!/bin/sh
source /etc/agent/scripts/common.rc
commandresponsedoc=$2
commandrequestdoc=$1
requestname=$3

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts

responsename=${requestname%Request}
sed -ie s/xmlns=/xmlns_new=/ ${commandrequestdoc}

xsltproc -o ${commandresponsedoc} \
	--stringparam tojid $jid \
	--stringparam innerresponsename $responsename"Response" \
	--stringparam responsename $responsename"ResponseData" \
	--stringparam requestfile ${commandrequestdoc} \
	${AGENT_XSL_DIR}/devicecommand.xsl \
	${AGENT_TEMPLATES_DIR}/DeviceCommand.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${commandresponsedoc}
case ${requestname} in
	RebootDeviceRequest)
		/V2O/bin/reboot_system
		;;
	ShutdownDeviceRequest)
		/V2O/bin/shutdown_system
		;;
	SaveConfigDeviceRequest)
		/V2O/bin/save_config
		;;
	RevertToDefaultDeviceRequest)
		/V2O/bin/revert_config
		;;
	*)
		echo "Unknown Command " ${requestname}
		;;
esac

