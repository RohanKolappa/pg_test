<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <EthernetPortTableConfig>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <xsl:apply-templates/>
      </EthernetPortTableConfig>
    </xsl:template>

    <xsl:template match="record">
        <EthernetPortConfig>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@jsxDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="ListIndex">
                <xsl:value-of select="@jsxListIndex"/>
            </xsl:attribute>
            <PortID>
               <xsl:value-of select="@jsxLanPortId"/>
            </PortID> 
            <EnableInterface>
                <xsl:choose>
                  <xsl:when test="@jsxLanIsEnable='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
            </EnableInterface>
            <IsPrimary>
                <xsl:choose>
                  <xsl:when test="@jsxLanIsPrimary='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
            </IsPrimary>
            <EnableMulticast>
                <xsl:choose>
                  <xsl:when test="@jsxLanMulticast='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
            </EnableMulticast>
            <MTU>
              <xsl:value-of select="@jsxLanMtu"/>
            </MTU>
          <IPConfig>
                <UseDHCP>
                    <xsl:choose>
                      <xsl:when test="@jsxLanDhcp='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                    </xsl:choose>
                </UseDHCP>
                <IPAddress>
                   <xsl:value-of select="@jsxLanIPAddress"/>
                </IPAddress>
                <Netmask>
                   <xsl:value-of select="@jsxLanNetmask"/>
                </Netmask>
            </IPConfig>
        </EthernetPortConfig>
    </xsl:template>
 </xsl:stylesheet>
