#/bin/ash
cat <<DescribeMSServerOptions
<MenuDescription>
   <Type>config</Type>
   <Title>MSServerOptions</Title>
   <Description>Management Server Options</Description>
   <BoardAccess>false</BoardAccess>
   <CliCommandName>ms_server_options</CliCommandName>

   <MenuItems>
      <Attribute name="enablediscovery" label="Allow media devices to auto register" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="enableuserregistry" label="Users can create their own account" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="enablentp" label="Enable NTP Server" type="boolean" size="8" defaultvalue="No" />
      <Attribute name="ntpip" label="NTP Server" type="string" size="32" defaultvalue="" validation="ipaddress" />
   </MenuItems>
</MenuDescription>
DescribeMSServerOptions

