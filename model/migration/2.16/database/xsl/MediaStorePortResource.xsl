
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.17</xsl:attribute>
    </xsl:template>
	<xsl:template match="//Info/Type">
	        <xsl:choose>
	            <xsl:when test=".!='Both' ">
	               <Type>Both</Type>
	            </xsl:when>
	            <xsl:otherwise>
	               <xsl:copy-of select='.' />
	            </xsl:otherwise>
	        </xsl:choose>
	</xsl:template>
</xsl:stylesheet>
