<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="NATZoneTableConfig">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="NATZoneTableConfig/NATZoneEntry">
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
        <xsl:attribute name="jsxNatName">
            <xsl:value-of select="$record_el/Name"/>
        </xsl:attribute>
        <xsl:attribute name="jsxNatIPAddress">
            <xsl:value-of select="$record_el/IPAddress"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>