<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
       <data jsxid="jsxroot">
         <xsl:apply-templates/>
       </data>
    </xsl:template>
   
    <xsl:template match="Tag/Value">
       <record>
          <xsl:attribute name="jsxid"><xsl:value-of select="."/></xsl:attribute>
          <xsl:attribute name="jsxtext"><xsl:value-of select="."/></xsl:attribute>
       </record>
    </xsl:template>
</xsl:stylesheet>