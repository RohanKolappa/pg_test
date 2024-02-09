#/bin/ash
cat <<DescribeLDAPConfiguration
<MenuDescription>

   <Type>config</Type>
   <Title>LDAPConfiguration</Title>
   <Description>LDAP Configuration</Description>
   <BoardAccess>false</BoardAccess>
   <CliCommandName>ldap_configuration</CliCommandName>

   <MenuItems>
      <Attribute name="enableldap" label="LDAP Authentication" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="ldaphost" label="LDAP Server IP/Name" type="string" size="45" defaultvalue="0.0.0.0" />
      <Attribute name="admindn" label="Admin DN" type="string" size="64" defaultvalue="" />
      <Attribute name="adminpassword" label="Admin Password" type="password" size="20" defaultvalue="" />
      <Attribute name="basedn" label="Base DN" type="string" size="256" defaultvalue="" />
      <Attribute name="userfield" label="User Name Field" type="string" size="20" defaultvalue="sAMAccountName" />

   </MenuItems>
</MenuDescription>
DescribeLDAPConfiguration
