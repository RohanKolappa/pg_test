<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="IPVSClientDisplayConfig|MRDecoderDisplayConfig">
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
            <xsl:copy-of select="Canvas"/>
        </record>
      </data>  
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">"IPVSClientDisplayConfig1"</xsl:attribute>
        <xsl:attribute name="jsxtext"></xsl:attribute>      
        <xsl:attribute name="jsxPriority"></xsl:attribute>   
        <xsl:attribute name="jsxDescription"></xsl:attribute> 
     </xsl:template>
</xsl:stylesheet>