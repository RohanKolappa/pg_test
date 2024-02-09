<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

	 <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.18</xsl:attribute>
    </xsl:template>

	<xsl:template match="/MediaStreamDirResource/Info/MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority">
		 <xsl:choose>
			<xsl:when test="string-length(normalize-space(.))=0">
	            <xsl:attribute name="priority">0</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
	            <xsl:copy-of select="."></xsl:copy-of>
            </xsl:otherwise>
        </xsl:choose>            
	</xsl:template>
</xsl:stylesheet>
 