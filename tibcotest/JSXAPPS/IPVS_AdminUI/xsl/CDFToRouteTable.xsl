<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <RouteTableConfig>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <xsl:apply-templates/>
      </RouteTableConfig>
    </xsl:template>

    <xsl:template match="record">
        <RouteEntry>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@jsxDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="ListIndex">
                <xsl:value-of select="@jsxListIndex"/>
            </xsl:attribute>
            <Destination>
               <xsl:value-of select="@jsxAssetRouteIPAddress"/>
            </Destination> 
            <Netmask>
               <xsl:value-of select="@jsxAssetRouteNetmask"/>
            </Netmask>
            <Gateway>
               <xsl:value-of select="@jsxAssetRouteGateway"/>
            </Gateway>
        </RouteEntry>
    </xsl:template>
 </xsl:stylesheet>
