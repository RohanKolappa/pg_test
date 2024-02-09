#!/bin/sh
source /etc/agent/scripts/common.rc
deviceloginrequest=$1
get_network_ports_status > ${PORT_STATUS}

if [ ! -f "${DB_DIR}/StreamInfoList.xml" ]; then
	cp ${AGENT_TEMPLATES_DIR}/StreamInfoList.xml ${DB_DIR}/StreamInfoList.xml
fi
if [ ! -f ${deviceloginrequest} ]; then
	if [ x"${portinfo}" = "xStreamDst" ]; then
		echo "action=add id=2002 operation=output streamtype=V2D dsttype=V2DHardWarePort ioport=1"   | nc 127.0.0.1 10001
		echo "action=start id=2002 queueid=2001 operation=output streamtype=V2D dsttype=V2DHardWarePort" | nc 127.0.0.1 10001
	else
		echo "action=add id=2001 operation=input streamtype=V2D srctype=V2DEncoderPort ioport=1"  | nc 127.0.0.1 10001
		echo "action=start id=2001 queueid=2001 operation=input streamtype=V2D srctype=V2DEncoderPort ioport=1"| nc 127.0.0.1 10001
	fi
fi

#create devicelogin request xml
xsltproc -o ${deviceloginrequest} --stringparam version $swver --stringparam serviceversion $serviceversion --stringparam password $password --stringparam jid $jid --stringparam devicetype $devicetype --stringparam portinfo $portinfo --stringparam statusfile ${PORT_STATUS} --stringparam streaminfolist ${DB_DIR}/StreamInfoList.xml ${AGENT_XSL_DIR}/deviceloginrequest.xsl ${AGENT_TEMPLATES_DIR}/DeviceLoginRequest.xml
sed -ie 's/<x>/<x xmlns="com.barco.agentmessage">/' ${deviceloginrequest}
#/bin/rm -rf /etc/agent/db/streamcache/*
echo ${devicename}@${serverip}/${devicename}
echo ${password}
echo ${serverip}
echo ${register}

