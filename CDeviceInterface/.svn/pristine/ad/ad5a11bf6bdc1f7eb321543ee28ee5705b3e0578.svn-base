#!/bin/sh
source /etc/agent/scripts/common.rc
deviceupgraderequest=$1
deviceupgraderesponse=$2
#upgradeURL="NONE"
AGENT_XSL_DIR=/etc/agent/xsl
AGENT_TEMPLATES_DIR=/etc/agent/templates

SCRIPTS_DIR=/etc/scripts

sed -ie s/xmlns=/xmlns_new=/ ${deviceupgraderequest}

xsltproc ${AGENT_XSL_DIR}/get_upgradeurl.xsl ${deviceupgraderequest} > /tmp/upgradeurl
source /tmp/upgradeurl
if [ x"$url" != "x" ]; then	
	/V2O/bin/upgrade_device -url ${url}
		if [ "x$?" != "x0" ]; then
			echo "Invalid URL"
			xsltproc -o /tmp/barcoerror.xml  \
				--stringparam code 2902 \
			    --stringparam description 'Execution of command failed' \
				${AGENT_XSL_DIR}/update_error_description_code.xsl  \
				${AGENT_TEMPLATES_DIR}/BarcoError.xml
		
		xsltproc -o ${deviceupgraderesponse}  \
			--stringparam requestfile $deviceupgraderequest \
	        --stringparam jid $jid \
	        --stringparam state 0 \
	    	--stringparam errorfile /tmp/barcoerror.xml \
			${AGENT_XSL_DIR}/upgradedeviceresponse_error.xsl  \
			${AGENT_TEMPLATES_DIR}/UpgradeDeviceResponse.xml
		/bin/rm -f /tmp/barcoerror.xml
		/bin/rm -f /tmp/upgradeurl
		else
				xsltproc -o ${deviceupgraderesponse}  \
					--stringparam requestfile $deviceupgraderequest \
				    --stringparam jid $jid \
					${AGENT_XSL_DIR}/upgradedeviceresponse.xsl  \
					${AGENT_TEMPLATES_DIR}/UpgradeDeviceResponse.xml
			sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${deviceupgraderesponse}
		fi
fi