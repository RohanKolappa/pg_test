<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="deviceTemplate">
        <xsl:with-param name="device_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="deviceTemplate">
      <xsl:param name="device_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$device_el"/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>
    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkNASTable">
            <xsl:value-of select="@DirtyBitblkNASTable"/>
        </xsl:attribute> 
     </xsl:template>
</xsl:stylesheet>