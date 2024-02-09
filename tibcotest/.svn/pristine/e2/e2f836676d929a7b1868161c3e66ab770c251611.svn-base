<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="natTemplate">
        <xsl:with-param name="nat_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="natTemplate">
      <xsl:param name="nat_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$nat_el"/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>
    
    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="parentNID">
            <xsl:value-of select="$record_el/@parentNID"/>
        </xsl:attribute>
        <xsl:attribute name="jsxXMLVer">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="txtXMLRev">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxNATPolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxNATPolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxNATPolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxNATPolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxNATLANID">
            <xsl:choose>
                <xsl:when test="$record_el/NetworkAddressScheme/EthernetPortEntryID!=''">
                    <xsl:value-of select="$record_el/NetworkAddressScheme/EthernetPortEntryID"/>
                </xsl:when>
                <xsl:otherwise>Select LANID</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <xsl:attribute name="jsxNATZoneEntryName">
            <xsl:value-of select="$record_el/NetworkAddressScheme/NATZoneEntryName"/>
        </xsl:attribute>
    </xsl:template>
</xsl:stylesheet>