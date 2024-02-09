<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>

<xsl:template match="/*/@serviceVer">
  <xsl:attribute name="serviceVer">
    <xsl:text>2.5</xsl:text>
  </xsl:attribute>
</xsl:template>

<!-- Here's the boilerplate code -->
<!-- By default, copy all nodes unchanged -->
<xsl:template match="@* | node()">
  <xsl:apply-templates mode="before" select="."/>
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsl:apply-templates mode="add-atts" select="."/>
    <xsl:apply-templates mode="insert" select="."/>
    <xsl:apply-templates/>
    <xsl:apply-templates mode="append" select="."/>
  </xsl:copy>
  <xsl:apply-templates mode="after" select="."/>
</xsl:template>

<!-- By default, don't add anything -->
<xsl:template mode="add-atts" match="*"/>
<xsl:template mode="insert" match="*"/>
<xsl:template mode="append" match="*"/>
<xsl:template mode="before" match="@* | node()"/>
<xsl:template mode="after" match="@* | node()"/>

<!--Add Elements of Default player settings-->
  <xsl:template mode="append" match="/Preferences/ClientApp[@name='IPVSClient']">
    <!-- bookmarks -->
    <xsl:choose>
      <xsl:when test="ShowBookmarks">
        <!-- do noting -->  
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
	<ShowBookmarks>true</ShowBookmarks>
      </xsl:otherwise>
    </xsl:choose>

    <!-- Connections -->
   <xsl:choose>
      <xsl:when test="ShowConnections">
        <!-- do noting -->  
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
	<ShowConnections>true</ShowConnections>
      </xsl:otherwise>
    </xsl:choose>

  <!-- Controller -->
   <xsl:choose>
      <xsl:when test="ShowController">
        <!-- do noting -->  
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
	<ShowController>true</ShowController>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

