<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>

<xsl:template match="/*/@serviceVer">
  <xsl:attribute name="serviceVer">
    <xsl:text>2.28.10</xsl:text>
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

<!--add element 'resourcePassword' under 'Login' -->
 <xsl:template mode="append" match="/Preferences/Login">
    <!-- resourcePassword -->
    <xsl:choose>
      <xsl:when test="resourcePassword">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'resourcePassword' node -->
	    <resourcePassword></resourcePassword>
      </xsl:otherwise>
    </xsl:choose> 

    <!--add element 'UnmannedMode' under 'Login' -->
    <!-- UnmannedMode -->
    <xsl:choose>
      <xsl:when test="UnmannedMode">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'UnmannedMode' node -->
	    <UnmannedMode>false</UnmannedMode>
      </xsl:otherwise>
    </xsl:choose>
     
    <!--add element 'AutoGenerateResourcePassword' under 'Login' -->
    <!-- AutoGenerateResourcePassword -->
    <xsl:choose>
      <xsl:when test="AutoGenerateResourcePassword">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'AutoGenerateResourcePassword' node -->
	    <AutoGenerateResourcePassword>true</AutoGenerateResourcePassword>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


</xsl:stylesheet>

