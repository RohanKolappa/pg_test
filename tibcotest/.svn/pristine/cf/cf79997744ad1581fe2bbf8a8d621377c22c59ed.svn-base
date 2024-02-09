<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="Groups">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="Groups/Tag">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@Name"/>
            <xsl:value-of select="$record_el/@Value"/>
        </xsl:attribute>
        <xsl:attribute name="jsxTagName">
            <xsl:value-of select="$record_el/@Name"/>
        </xsl:attribute>      
        <xsl:attribute name="jsxTagValue">
            <xsl:value-of select="$record_el/@Value"/>
        </xsl:attribute> 
        <xsl:attribute name="jsxtext">
            <xsl:value-of select="$record_el/@Name"/> = <xsl:value-of select="$record_el/@Value"/>
        </xsl:attribute>   
     </xsl:template>
</xsl:stylesheet>