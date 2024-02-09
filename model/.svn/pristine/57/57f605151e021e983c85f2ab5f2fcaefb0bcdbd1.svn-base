
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.11</xsl:attribute>
    </xsl:template>
    <xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.4</xsl:attribute>
    </xsl:template>

    <xsl:template mode="after" match="//ModifiedDate">
        <xsl:choose>
            <xsl:when test="../UserPrivateData">
            </xsl:when>
            <xsl:otherwise>
				<UserPrivateData/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>
