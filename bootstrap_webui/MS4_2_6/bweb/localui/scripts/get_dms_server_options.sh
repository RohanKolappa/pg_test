#/bin/ash

ISCOMBO=`/V2O/bin/get_xmpp_server | grep 'name="enable"' | cut -f4 -d'"'`
local DEVICETYPE=''
if [ "$ISCOMBO" = "Yes" ]; then
	DEVICETYPE="MS/DMS Combo"
elif [ "$ISCOMBO" = "No" ]; then
	DEVICETYPE="DMS"
fi

ISAGENTENABLED=`/V2O/bin/get_xmpp_agent | grep 'name="enable"' | cut -f4 -d'"'`
local AUTODETECT=''
if [ "$ISAGENTENABLED" = "Yes" ]; then
	AUTODETECT="No"
elif [ "$ISAGENTENABLED" = "No" ]; then
	AUTODETECT="Yes"
fi

SERVERIP=`/V2O/bin/get_xmpp_agent | grep 'name="serverip"' | cut -f4 -d'"'`
NTPENABLE=`/V2O/bin/get_ntp_server | grep 'name="enable"' | cut -f4 -d'"'`
NTPIPADDRESS=`/V2O/bin/get_ntp | grep 'name="ipaddress"' | cut -f4 -d'"'`
ENABLEDEVICEDISCOVERY=`/V2O/bin/get_xmpp_server | grep 'name="enablediscovery"' | cut -f4 -d'"'`
ENABLEUSERREGISTRATION=`/V2O/bin/get_xmpp_server | grep 'name="enableuserregistry"' | cut -f4 -d'"'`

cat <<GetDMSServerOptions
<Group name="Get DMS Server Options">
  <Response>
    <Attribute name="Status" value="Success"></Attribute>
    <Attribute name="Reason" value="N/A"></Attribute>
  </Response>
  <ObjectEntry name="DMSServerOptions">
      <Attribute name="deviceType" value="${DEVICETYPE}"></Attribute>
      <Attribute name="serverip" value="${SERVERIP}"></Attribute>
      <Attribute name="autodetect" value="${AUTODETECT}"></Attribute>
      <Attribute name="enablentp" value="${NTPENABLE}"></Attribute>
      <Attribute name="ntpip" value="${NTPIPADDRESS}"></Attribute>
      <Attribute name="enablediscovery" value="${ENABLEDEVICEDISCOVERY}"></Attribute>
      <Attribute name="enableuserregistry" value="${ENABLEUSERREGISTRATION}"></Attribute>
  </ObjectEntry>
</Group>
GetDMSServerOptions
