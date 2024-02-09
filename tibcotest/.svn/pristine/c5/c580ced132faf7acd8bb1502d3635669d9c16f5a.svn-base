<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="routingTemplate">
        <xsl:with-param name="routing_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="routingTemplate">
      <xsl:param name="routing_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$routing_el"/>
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
        <xsl:attribute name="jsxStreamFilterXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxStreamFilterXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxStreamFilterXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyStreamFilterName">
            <xsl:value-of select="$record_el/Name"/>
        </xsl:attribute>
    </xsl:template>
</xsl:stylesheet>