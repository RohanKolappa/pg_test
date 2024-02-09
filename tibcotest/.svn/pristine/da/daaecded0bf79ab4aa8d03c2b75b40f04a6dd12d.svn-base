<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="Session">
    <NIDList>
      <xsl:for-each select="./EntityList/Entity[@type='SrcPort']">
        <xsl:choose>
          <xsl:when test="position()>1">,<xsl:value-of select="normalize-space(Port/@resourceNID)"/></xsl:when>
          <xsl:otherwise><xsl:value-of select="normalize-space(Port/@resourceNID)"/></xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
    </NIDList>
  </xsl:template>
</xsl:stylesheet>