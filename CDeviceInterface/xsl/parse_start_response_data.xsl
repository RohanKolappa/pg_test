<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

 <xsl:output method="text" />

 <!-- Get the Version attribute from the root node of input document -->
  <xsl:template match="/">
	<xsl:text>roomnid=</xsl:text>
    <xsl:value-of select="//@roomNID"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>streamnid=</xsl:text>
    <xsl:value-of select="//@streamNID"/>
    <xsl:text>&#10;</xsl:text>
	<xsl:text>url=</xsl:text>
    <xsl:value-of select="//URL"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>relayroom=</xsl:text>
    <xsl:value-of select="//@relayRoomNID"/>
    <xsl:text>&#10;</xsl:text>
     <xsl:text>relaystream=</xsl:text>
    <xsl:value-of select="//@relayStreamNID"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>toagentjid=</xsl:text>
    <xsl:value-of select="//@to"/>
	<xsl:text>&#10;</xsl:text>
  </xsl:template>

</xsl:stylesheet>

