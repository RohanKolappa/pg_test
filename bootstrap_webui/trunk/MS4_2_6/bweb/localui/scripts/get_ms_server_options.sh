#/bin/ash
ENABLEDISCOVERY=`/V2O/bin/get_xmpp_server | grep 'name="enablediscovery"' | cut -f4 -d'"'`
ENABLEUSERREGISTRY=`/V2O/bin/get_xmpp_server | grep 'name="enableuserregistry"' | cut -f4 -d'"'`
ENABLENTPSERVER=`/V2O/bin/get_ntp_server | grep 'name="enable"' | cut -f4 -d'"'`
NTPIPADDRESS=`/V2O/bin/get_ntp | grep 'name="ipaddress"' | cut -f4 -d'"'`

cat <<GetMSServerOptions
<Group name="Get MS Server Options">
  <Response>
    <Attribute name="Status" value="Success"></Attribute>
    <Attribute name="Reason" value="N/A"></Attribute>
  </Response>
  <ObjectEntry name="MSServerOptions">
      <Attribute name="enablediscovery" value="${ENABLEDISCOVERY}"></Attribute>
      <Attribute name="enableuserregistry" value="${ENABLEUSERREGISTRY}"></Attribute>
      <Attribute name="enablentp" value="${ENABLENTPSERVER}"></Attribute>
      <Attribute name="ntpip" value="${NTPIPADDRESS}"></Attribute>
  </ObjectEntry>
</Group>
GetMSServerOptions

