
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.16</xsl:attribute>
    </xsl:template>
	<xsl:template match="//StreamTypeList/StreamType">
	        <xsl:choose>
	            <xsl:when test="//MediaStoreFileInfo[Type='MediaClip'] and .='' ">
	               <StreamType>V2D</StreamType>
	            </xsl:when>
	            <xsl:otherwise>
	               <xsl:copy-of select='.' />
	            </xsl:otherwise>
	        </xsl:choose>
	</xsl:template>
</xsl:stylesheet>
