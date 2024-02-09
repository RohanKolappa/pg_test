<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="GetTagsResponse">
        <data>
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="GetTagsResponse/Tag">
        <tag>
         <xsl:variable name="tagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
		<tagName><xsl:value-of select="$tagName"/></tagName>
		<tagValue>
		<data jsxid="jsxroot">
		<xsl:for-each select="Value">
		<record>
		<xsl:attribute name="jsxTagName"><xsl:value-of select="$tagName"/></xsl:attribute>
		<xsl:attribute name="jsxid"><xsl:value-of select="."/></xsl:attribute>
		<xsl:attribute name="jsxTagValue"><xsl:value-of select="."/></xsl:attribute>
		</record>
		</xsl:for-each>
		</data>
		</tagValue>
        </tag>
    </xsl:template>
</xsl:stylesheet>