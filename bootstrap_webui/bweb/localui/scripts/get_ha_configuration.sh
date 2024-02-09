#/bin/ash
ENABLEHA=`/V2O/bin/get_ha | grep 'name="enable"' | cut -f4 -d'"'`
PEERIP=`/V2O/bin/get_ha | grep 'name="peerip"' | cut -f4 -d'"'`
PEERNAME=`/V2O/bin/get_ha | grep 'name="peername"' | cut -f4 -d'"'`
INTERFACE=`/V2O/bin/get_ha | grep 'name="interface"' | cut -f4 -d'"'`
ENABLEVIRTUALIP=`/V2O/bin/get_ha | grep 'name="enable_virtual_ip"' | cut -f4 -d'"'`
VIRTUALIP=`/V2O/bin/get_ha | grep 'name="virtualip"' | cut -f4 -d'"'`
VIRTUALIP2=`/V2O/bin/get_ha | grep 'name="virtualip2"' | cut -f4 -d'"'`
ISPRIMARY=`/V2O/bin/get_ha | grep 'name="enable_as_primary"' | cut -f4 -d'"'`
local ENABLEDNS=''
if [ "$ENABLEVIRTUALIP" = "Yes" ]; then
	ENABLEDNS="No"
elif [ "$ENABLEVIRTUALIP" = "No" ]; then
	ENABLEDNS="Yes"
fi

local ISSECONDARY=''
if [ "$ISPRIMARY" = "Yes" ]; then
	ISSECONDARY="No"
elif [ "$ISPRIMARY" = "No" ]; then
	ISSECONDARY="Yes"
fi

cat <<GetHAConfiguration
<Group name="Get HA Configuration">
  <Response>
    <Attribute name="Status" value="Success"></Attribute>
    <Attribute name="Reason" value="N/A"></Attribute>
  </Response>
  <ObjectEntry name="HAConfiguration">
      <Attribute name="enableha" value="${ENABLEHA}"></Attribute>
      <Attribute name="peerip" value="${PEERIP}"></Attribute>
      <Attribute name="peername" value="${PEERNAME}"></Attribute>
      <Attribute name="interface" value="${INTERFACE}"></Attribute>
      <Attribute name="enable_virtual_ip" value="${ENABLEVIRTUALIP}"></Attribute>
      <Attribute name="virtualip" value="${VIRTUALIP}"></Attribute>
      <Attribute name="virtualip2" value="${VIRTUALIP2}"></Attribute>
      <Attribute name="enabledns" value="${ENABLEDNS}"></Attribute>
      <Attribute name="enable_as_secondary" value="${ISSECONDARY}"></Attribute>
  </ObjectEntry>
</Group>

GetHAConfiguration

