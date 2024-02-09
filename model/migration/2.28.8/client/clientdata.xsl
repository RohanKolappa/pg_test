<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>

<xsl:template match="/*/@serviceVer">
  <xsl:attribute name="serviceVer">
    <xsl:text>2.28.9</xsl:text>
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

<!--add element 'ClientApp name="IPVSAdmin"' under 'Login' -->
 <xsl:template mode="append" match="/Preferences">
    <!-- ClientApp name="IPVSAdmin" -->
    <xsl:choose>
      <xsl:when test="ClientApp[@name='IPVSAdmin']">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'ClientApp name="IPVSAdmin"' node -->
        <ClientApp name="IPVSAdmin">
          <CustomizeList>
            <Asset></Asset>
            <User></User>
            <MediaStreamFile></MediaStreamFile>
            <ConnectionProfile></ConnectionProfile>
          </CustomizeList>
        </ClientApp>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

