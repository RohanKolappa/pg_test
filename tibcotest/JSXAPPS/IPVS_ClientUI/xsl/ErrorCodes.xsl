<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:gicache="http://xsd.tns.tibco.com/gi/cache">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
  <xsl:template match="@* | node()">
    <xsl:apply-templates mode="before" select="." />
    <xsl:copy>
      <xsl:apply-templates select="@*" />
      <xsl:apply-templates mode="add-atts" select="." />
      <xsl:apply-templates mode="insert" select="." />
      <xsl:apply-templates />
      <xsl:apply-templates mode="append" select="." />
    </xsl:copy>
    <xsl:apply-templates mode="after" select="." />
  </xsl:template>
  <!-- By default, don't add anything -->
  <xsl:template mode="add-atts" match="*" />
  <xsl:template mode="insert" match="*" />
  <xsl:template mode="append" match="*" />
  <xsl:template mode="before" match="@* | node()" />
  <xsl:template mode="after" match="@* | node()" />

  <xsl:template match="/gicache:loading">
    <loading xmlns="http://xsd.tns.tibco.com/gi/cache"/>
  </xsl:template>

  <xsl:template match="/gicache:timeout">
    <timeout xmlns="http://xsd.tns.tibco.com/gi/cache"/>
  </xsl:template>

  <xsl:template match="/gicache:error" >
    <xsl:call-template name="copy-error">
      <xsl:with-param name="error_code" select="substring-before(substring-after(@error,' returned status '),'.')" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="copy-error">
      <xsl:param name="error_code"/>
      <xsl:element name="data"> 
	<xsl:attribute name="jsxid">jsxroot</xsl:attribute>
	<xsl:element name="record"> 
	   <xsl:attribute name="name">jsxtext</xsl:attribute>
        <xsl:attribute name="jsxtext">
          <xsl:choose><xsl:when test="$error_code=400">error: hi there 0 </xsl:when>
            <xsl:when test="$error_code=401">error: hi there 1 </xsl:when>
            <xsl:when test="$error_code=402">error: hi there 2</xsl:when>
            <xsl:when test="$error_code=403">error: hi there 3</xsl:when>
            <xsl:when test="$error_code=3014">No Permissions availabale</xsl:when>
            <xsl:otherwise>unknown error</xsl:otherwise></xsl:choose>
        </xsl:attribute>
	</xsl:element>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>
