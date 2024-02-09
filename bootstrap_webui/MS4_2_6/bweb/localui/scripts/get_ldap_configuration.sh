#/bin/ash
ENABLELDAP=`/V2O/bin/get_xmpp_server | grep 'name="enableldap"' | cut -f4 -d'"'`
LDAPHOST=`/V2O/bin/get_xmpp_server | grep 'name="ldaphost"' | cut -f4 -d'"'`
ADMINDN=`/V2O/bin/get_xmpp_server | grep 'name="admindn"' | cut -f4 -d'"'`
ADMINPASSWORD=`/V2O/bin/get_xmpp_server | grep 'name="adminpassword"' | cut -f4 -d'"'`
BASEDN=`/V2O/bin/get_xmpp_server | grep 'name="basedn"' | cut -f4 -d'"'`
USERFIELD=`/V2O/bin/get_xmpp_server | grep 'name="userfield"' | cut -f4 -d'"'`

cat <<GetLDAPConfiguration
<Group name="Get LDAP Configuration">
  <Response>
    <Attribute name="Status" value="Success"></Attribute>
    <Attribute name="Reason" value="N/A"></Attribute>
  </Response>
  <ObjectEntry name="LDAPConfiguration">
      <Attribute name="enableldap" value="${ENABLELDAP}"></Attribute>
      <Attribute name="ldaphost" value="${LDAPHOST}"></Attribute>
      <Attribute name="admindn" value="${ADMINDN}"></Attribute>
      <Attribute name="adminpassword" value="${ADMINPASSWORD}"></Attribute>
      <Attribute name="basedn" value="${BASEDN}"></Attribute>
      <Attribute name="userfield" value="${USERFIELD}"></Attribute>
  </ObjectEntry>
</Group>
GetLDAPConfiguration

