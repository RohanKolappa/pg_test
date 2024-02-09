#!/bin/sh
source /etc/agent/scripts/common.rc
commandresponsedoc=$2
commandrequestdoc=$1
requestname=$3

AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts

jid=${devicename}@localhost/${devicename}
responsename=${requestname%Request}
sed -ie s/xmlns=/xmlns_new=/ ${commandrequestdoc}
xsltproc -o /tmp/apply.xsl --stringparam rootname `xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml` /etc/scripts/xsl/deviceconfig.xsl  /home/onair/objectXml/confighandler.xml

#remove all nodes that have blank "Dirty" flags
xsltproc -o /tmp/configupdate.xml /etc/agent/xsl/deviceupdate.xsl ${commandrequestdoc}
xsltproc /tmp/apply.xsl /tmp/configupdate.xml | sed -e 's/false/No/g' | sed -e 's/true/Yes/g' > /tmp/apply.sh
/bin/sh -x /tmp/apply.sh > /var/log/deviceapp/apply_config.log 2>&1

cp /mnt/cf/Device.xml /tmp/Device.xml.bak
/V2O/bin/save_config
cp /tmp/Device.xml.bak /mnt/cf/Device.xml

	xsltproc -o ${commandresponsedoc} \
                    --stringparam tojid $jid \
                    --stringparam innerresponsename $responsename"Response" \
                    --stringparam responsename $responsename"ResponseData" \
                    --stringparam requestfile ${commandrequestdoc} \
                    ${AGENT_XSL_DIR}/devicecommand.xsl \
                    ${AGENT_TEMPLATES_DIR}/DeviceCommand.xml
        sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${commandresponsedoc}
