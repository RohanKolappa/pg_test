<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes"/>
  <xsl:template match="/*/@serviceVer">
    <xsl:attribute name="serviceVer">
      <xsl:text>1.3</xsl:text>
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

  <!-- Add ATTRIBUTES Preferences ( were absent in 'objectXml_1.0' but added to 'objectXml_1.3') -->

  <xsl:template mode="add-atts" match="/Preferences">
    <xsl:choose>
      <xsl:when test="@serviceVer">
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="serviceVer">1.3</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Add Elements -->
  <xsl:template mode="append" match="/Preferences">
    <xsl:choose>
      <xsl:when test="Service">
        <!-- do noting -->  
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
        <Service>
          <servicedomain>default</servicedomain>
        </Service>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>