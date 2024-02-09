<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <DeviceConfig>
              <NetworkElementsConfig>
                  <DNSClientConfig>
                      <xsl:attribute name="Dirty">
                          <xsl:value-of select="@DirtyBitblkAssetDNSSettings"/>
                      </xsl:attribute>
                      <HostName>
                          <xsl:value-of select="@V2DHostname"/>
                      </HostName> 
                      <DNSDomain>
                          <xsl:value-of select="@V2DDomainName"/>
                      </DNSDomain>
                      <DNSServerIP>
                        <xsl:value-of select="@V2DDNS"/>
                      </DNSServerIP>
                  </DNSClientConfig>
                  <RouteTableConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkNetworkRouteTable"/>
                    </xsl:attribute>
                  </RouteTableConfig> 
                  <HostTableConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkHostTable"/>
                    </xsl:attribute>
                  </HostTableConfig> 
                  <NATZoneTableConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkNatTable"/>
                    </xsl:attribute>
                  </NATZoneTableConfig> 
                  <EthernetPortTableConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkLanTable"/>
                    </xsl:attribute>
                  </EthernetPortTableConfig> 
              </NetworkElementsConfig>
              <SystemElementsConfig>
                <SystemClockConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkNTPInfo"/>
                    </xsl:attribute>
                  <EnableNTPClient>
                    <xsl:choose>
                      <xsl:when test="@EnableNTPClient='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                    </xsl:choose>
                  </EnableNTPClient> 
                  <NTPServerIP>
                      <xsl:value-of select="@NTPServerIP"/>
                  </NTPServerIP>
                </SystemClockConfig>
              </SystemElementsConfig>
              <ServiceElementsConfig>
                <NTPServiceConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkNTPServer"/>
                    </xsl:attribute>
                  <EnableNTPServer>
                    <xsl:choose>
                      <xsl:when test="@EnableServerNTPServer='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                    </xsl:choose>
                  </EnableNTPServer> 
                  <NTPServerIP>
                      <xsl:value-of select="@NTPServerServerIP"/>
                  </NTPServerIP>
                </NTPServiceConfig>
              </ServiceElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
