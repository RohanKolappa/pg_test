<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

 <xsl:output method="text" />

 <!-- Get the Version attribute from the root node of input document -->
  <xsl:template match="/">
	<xsl:value-of select="//MediaPresenceEventData[@value='Ready' and @type='Entity']/@jid"/>
	<xsl:text>&#10;</xsl:text>
  </xsl:template>
</xsl:stylesheet>

