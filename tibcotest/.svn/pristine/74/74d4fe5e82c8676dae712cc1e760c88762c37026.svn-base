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
              <ServiceElementsConfig>
                <HAServiceConfig>
                      <HAServer>
                        <xsl:attribute name="Dirty">
                            <xsl:value-of select="@DirtyBitblkHAConfig"/>
                        </xsl:attribute>
                        <HAServerEnable>
                          <xsl:choose>
                            <xsl:when test="@jsxHAServerEnable='true' or @jsxHAServerEnable='1'">true</xsl:when>
                            <xsl:otherwise>false</xsl:otherwise>
                          </xsl:choose>
                        </HAServerEnable>
                        <PeerIP>
                            <xsl:value-of select="@jsxPeerIPAddress"/>
                        </PeerIP> 
                        <PeerName>
                            <xsl:value-of select="@jsxPeerName"/>
                        </PeerName>
                        <VirtualIPEnable>
                          <xsl:choose>
                            <xsl:when test="@jsxEnableVIP='true' or @jsxEnableVIP='1'">true</xsl:when>
                            <xsl:otherwise>false</xsl:otherwise>
                          </xsl:choose>
                        </VirtualIPEnable>
                        <VirtualIP>
                            <xsl:value-of select="@jsxVirtualIPAddress"/>
                        </VirtualIP>
                        <VirtualIP2>
                          <xsl:value-of select="@jsxVirtualIPAddress2"/>
                        </VirtualIP2>
                        <Primary>
                          <xsl:choose>
                            <xsl:when test="@jsxIsPrimary='true' or @jsxIsPrimary='Yes' or @jsxIsPrimary='1'" >true</xsl:when>
                            <xsl:otherwise>false</xsl:otherwise>
                          </xsl:choose>
                        </Primary>
                        <Interface>
                          <xsl:value-of select="@jsxHAInterface"/>
                        </Interface>
                      </HAServer>
                  </HAServiceConfig>
              </ServiceElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
