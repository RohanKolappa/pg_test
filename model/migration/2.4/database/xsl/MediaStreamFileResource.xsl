
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
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
 

    <xsl:template  match="@serviceVer">
    <xsl:attribute name="serviceVer"></xsl:attribute>
    </xsl:template>


<!-- Add ATTRIBUTES MediaStreamFileResource ( were absent in 'objectXml_2.4' but added to  'objectXml_2.5') -->

    <xsl:template mode="add-atts" match="/MediaStreamFileResource/Info/MediaStoreFileInfo/MediaGroupID">
      <xsl:choose>
          <xsl:when test="@ClipID">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="ClipID"></xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>



<!-- Add ELEMENTS MediaStreamFileResource ( were absent in 'objectXml_2.4' but added to  'objectXml_2.5') -->

    <!-- 
          /MediaStreamFileResource/Info/RecInfo
    -->

    <xsl:template mode="append" match="/MediaStreamFileResource/Info">
      <xsl:choose>
          <xsl:when test="RecInfo">
          </xsl:when>
          <xsl:otherwise>
              <RecInfo SessionConfigNID="" SessionID=""/>
          </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

</xsl:stylesheet>