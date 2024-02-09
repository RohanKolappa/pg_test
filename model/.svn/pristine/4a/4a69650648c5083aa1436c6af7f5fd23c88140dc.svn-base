
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


<!-- Add ATTRIBUTES MediaStreamDirResource ( were absent in 'objectXml_1.5' but added to  'objectXml_2.0') -->

    <xsl:template mode="add-atts" match="/MediaStreamDirResource/Info/MediaStorePortResourceNIDList">
      <xsl:choose>
          <xsl:when test="@synchMediaStorePortResourceNID">
          </xsl:when>
          <xsl:otherwise>
              <xsl:attribute name="synchMediaStorePortResourceNID"></xsl:attribute>
          </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

</xsl:stylesheet>