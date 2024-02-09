
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.14</xsl:attribute>
    </xsl:template>
    <xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.7</xsl:attribute>
    </xsl:template>

    <xsl:template mode="after" match="//FrameRateDivider">
        <xsl:choose>
            <xsl:when test="../IFrameInterval">
            </xsl:when>
            <xsl:otherwise>
				<IFrameInterval>0</IFrameInterval>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
