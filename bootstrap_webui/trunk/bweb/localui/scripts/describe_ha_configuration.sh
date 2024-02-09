#/bin/ash
cat <<DescribeHAConfiguration
<MenuDescription>

   <Type>config</Type>
   <Title>HA Configuration</Title>
   <Description>High Availability</Description>
   <BoardAccess>false</BoardAccess>
   <CliCommandName>ha_configuration</CliCommandName>
   <ConfigFile>/V2O/config/linuxhaconf</ConfigFile>

   <MenuItems>
      <Attribute name="enableha" label="Enable HA" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="peerip" label="Peer IP Address" type="string" size="32" defaultvalue="0.0.0.0" validation="ipaddress"/>
      <Attribute name="peername" label="Peer Node Name" type="string" size="32" defaultvalue="none" />
      <Attribute name="interface" label="LAN" type="string" size="6" defaultvalue="1"  validation="enumeration" values="1 , 2 ,"/>
      <Attribute name="enable_virtual_ip" label="Enable Virtual IP" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="virtualip" label="Virtual IP Address1" type="string" size="32" defaultvalue="0.0.0.0" />
      <Attribute name="virtualip2" label="Virtual IP Address2" type="string" size="32" defaultvalue="0.0.0.0" />
      <Attribute name="enabledns" label="Use DNS" type="boolean" size="4" defaultvalue="No" />
      <Attribute name="enable_as_secondary" label="Set As Secondary" type="boolean" size="4" defaultvalue="No" />
   </MenuItems>

</MenuDescription>

DescribeHAConfiguration


