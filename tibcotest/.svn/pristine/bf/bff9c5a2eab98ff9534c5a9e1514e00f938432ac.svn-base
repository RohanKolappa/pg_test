<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <NetworkAddressRule>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@jsxXMLRev"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@jsxXMLVer"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@jsxXMLServiceVersion"/>
          </xsl:attribute>
          <rulename>
              <xsl:value-of select="@jsxNATPolicyName"/>
          </rulename> 
          <order>
              <xsl:value-of select="@jsxNATPolicyOrder"/>
          </order>
          <valid>
            <xsl:choose>
              <xsl:when test="@jsxNATPolicyActive='true' or @jsxNATPolicyActive='1'">true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </valid>
          <LocalNetworkResourceFilterSet>
          </LocalNetworkResourceFilterSet>
          <RemoteNetworkResourceFilterSet>
          </RemoteNetworkResourceFilterSet>
          <RuleKeyTagFilterSet>
          </RuleKeyTagFilterSet>
          <NetworkAddressScheme>
            <UseDNSName>false</UseDNSName>
            <UseLANIP>false</UseLANIP>
            <UseNATZoneIP>false</UseNATZoneIP>
            <EthernetPortEntryID>
                <xsl:value-of select="@jsxNATLANID"/>
            </EthernetPortEntryID>
            <NATZoneEntryName>
                <xsl:value-of select="@jsxNATZoneEntryName"/>
            </NATZoneEntryName>
          </NetworkAddressScheme>
          <OutboundNATTable>
            <NATTable>
            </NATTable>
          </OutboundNATTable>
        </NetworkAddressRule>
    </xsl:template>
 </xsl:stylesheet>
