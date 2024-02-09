#!/bin/ash
echo Content-Type: text/xml
echo X-UA-Compatible: IE=edge,IE=11,IE=EmulateIE11
echo Cache-Control: no-cache, must-revalidate
echo
if [ x"${REQUEST_METHOD}" = "xGET" ] ; then
  XSLDIR="/etc/agent/xsl"
  TEMPLATEDIR="/etc/agent/templates"
  CONFIGDIR="/etc/config"
  STATUSDIR="/tmp/"
  type=`cat /etc/config/hw_profile_*.txt`
  title=`grep devicename /tmp/get_xmpp_agent.xml  | cut -f4 -d'"'`
  password=`grep password /tmp/get_xmpp_agent.xml  | cut -f4 -d'"'`
  if [ -f /tmp/status ]; then
    state="Registered"
  else
    state="UnRegistered"
  fi
  /V2O/bin/get_network_ports_status > /tmp/networkstatus.xml
  /V2O/bin/get_xmpp_agent_status > /tmp/xmppstatus.xml
  xsltproc -o /var/www/out/di.xml \
    --stringparam title ${title} \
    --stringparam password ${password}\
    --stringparam type ${type}\
    --stringparam state ${state} \
    --stringparam configfile ${CONFIGDIR}/Device.xml \
    --stringparam networkstatusfile /tmp/networkstatus.xml \
    --stringparam xmppstatusfile /tmp/xmppstatus.xml \
      ${XSLDIR}/di.xsl ${TEMPLATEDIR}/di.xml
  cat /var/www/out/di.xml
else
  rm -f /tmp/update.xml /tmp/configupdate.xml /tmp/apply.xsl /tmp/apply.sh
  date >> /tmp/device.log
  tee /tmp/update.xml >> /tmp/device.log
  xsltproc -o /tmp/apply.xsl --stringparam rootname "V2D-XPi-Device" /etc/scripts/xsl/deviceconfig.xsl  /home/onair/objectXml/confighandler.xml
  xsltproc -o /tmp/configupdate.xml /etc/agent/xsl/deviceupdate.xsl /tmp/update.xml
  xsltproc /tmp/apply.xsl /tmp/configupdate.xml | sed -e 's/false/No/g' | sed -e 's/true/Yes/g'  | sed -e 's/^/\/V2O\/bin\//' > /tmp/apply.sh
  /bin/sh /tmp/apply.sh > /var/log/deviceapp/apply_config.log 2>&1
  /V2O/bin/save_config
  cat /etc/config/Device.xml
fi
