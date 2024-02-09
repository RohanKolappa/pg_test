<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <NATZoneTableConfig>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <xsl:apply-templates/>
      </NATZoneTableConfig>
    </xsl:template>

    <xsl:template match="record">
        <NATZoneEntry>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@jsxDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="ListIndex">
                <xsl:value-of select="@jsxListIndex"/>
            </xsl:attribute>
            <Name>
               <xsl:value-of select="@jsxNatName"/>
            </Name> 
            <IPAddress>
               <xsl:value-of select="@jsxNatIPAddress"/>
            </IPAddress>
        </NATZoneEntry>
    </xsl:template>
 </xsl:stylesheet>
