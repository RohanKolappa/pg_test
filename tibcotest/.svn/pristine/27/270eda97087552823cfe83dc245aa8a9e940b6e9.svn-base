<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <data jsxid="jsxroot">
        <xsl:apply-templates/>
      </data>
    </xsl:template>
    <xsl:template match="GetTagsResponse/Tag">
        <record>
            <xsl:attribute name="jsxid">
                <xsl:value-of select="@Name"/>
            </xsl:attribute>
            <xsl:attribute name="jsxtext">
                <xsl:value-of select="@Name"/>
            </xsl:attribute>
           <xsl:element name="Tag">
                <xsl:copy-of select="Value"/>
           </xsl:element>
        </record>
    </xsl:template>
</xsl:stylesheet>