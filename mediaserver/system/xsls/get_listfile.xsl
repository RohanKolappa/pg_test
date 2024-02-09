<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

 <xsl:output method="text" />

 <!-- Get the Version attribute from the root node of input document -->
  <xsl:template match="/">
    <xsl:for-each select="//cmd[../action='ADD'][../device='V2D-XPi-Device']">
			<xsl:value-of select="."/>
			<xsl:text>:</xsl:text>
			<xsl:value-of select="../tablename"/>
			<xsl:text>:</xsl:text>
			<xsl:value-of select="../default"/>
			<xsl:text>&#10;</xsl:text>
	</xsl:for-each>
  </xsl:template>
</xsl:stylesheet>

