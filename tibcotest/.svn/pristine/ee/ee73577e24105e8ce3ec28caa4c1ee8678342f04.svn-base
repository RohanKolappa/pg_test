<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="RouteTableConfig">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="RouteTableConfig/RouteEntry">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxDirtyBit">
            <xsl:value-of select="$record_el/@Dirty"/>
        </xsl:attribute>
        <xsl:attribute name="jsxListIndex">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxAssetRouteIPAddress">
            <xsl:value-of select="$record_el/Destination"/>
        </xsl:attribute>
        <xsl:attribute name="jsxAssetRouteNetmask">
            <xsl:value-of select="$record_el/Netmask"/>
        </xsl:attribute>
        <xsl:attribute name="jsxAssetRouteGateway">
            <xsl:value-of select="$record_el/Gateway"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>