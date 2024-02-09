<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="xml" indent="yes" />

  <!-- Identity template -->
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>


  <!-- Update the Version at root node to next version -->
  <xsl:template match="/*/@Version">
    <xsl:attribute name="Version">
      <xsl:text>4.2</xsl:text>
    </xsl:attribute>
  </xsl:template>

</xsl:stylesheet>
