<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="GetTagsResponse">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="GetTagsResponse/Tag">
        <record>
            <xsl:variable name="tagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
            <xsl:attribute name="jsxid"><xsl:value-of select="concat('jsxTagValue',$tagName)"/></xsl:attribute>
            <xsl:attribute name="jsxColumnName">
                <xsl:value-of select="@Name"/>
            </xsl:attribute>
            <xsl:attribute name="jsxChkColumn">1</xsl:attribute>
            <xsl:attribute name="jsxColumnSortable">0</xsl:attribute>
        </record>
    </xsl:template>
</xsl:stylesheet>