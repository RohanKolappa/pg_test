
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.13</xsl:attribute>
    </xsl:template>
    <xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.6</xsl:attribute>
    </xsl:template>

    <xsl:template mode="append" match="//MediaStreamInfo">
        <xsl:choose>
            <xsl:when test="MediaStreamList">
            </xsl:when>
            <xsl:otherwise>
				<MediaStreamList/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
     <xsl:template mode="append" match="//Info">
        <xsl:choose>
            <xsl:when test="FeatureList">
            </xsl:when>
		    <xsl:otherwise>
				<FeatureList/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
