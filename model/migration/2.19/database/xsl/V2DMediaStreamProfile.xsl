
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.21</xsl:attribute>
    </xsl:template>
    <xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.5</xsl:attribute>
    </xsl:template>

    <xsl:template match="//Enable444">
        <xsl:choose>
            <xsl:when test=".='false'">
				<ColorSampling>4:2:2</ColorSampling>
            </xsl:when>
            <xsl:otherwise>
				<ColorSampling>Static 4:4:4</ColorSampling>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
