<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
	  <SEGMENTLIST>
      <xsl:apply-templates/>
	  </SEGMENTLIST>
  </xsl:template>

  <xsl:template match="data/record">
	  <SEGMENT>
      <xsl:attribute name="id">
        <xsl:value-of select="@jsxid"/>
      </xsl:attribute>
      <xsl:attribute name="inPoint">
        <xsl:value-of select="@jsxInPointTC"/>
      </xsl:attribute>
      <xsl:attribute name="outPoint">
        <xsl:value-of select="@jsxOutPointTC"/>
      </xsl:attribute>
	  </SEGMENT>
  </xsl:template> 
</xsl:stylesheet>