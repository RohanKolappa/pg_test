<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

	 <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.25</xsl:attribute>
    </xsl:template>

	<xsl:template match="//Canvas" mode="add-atts">
		  <xsl:attribute name="id" />
		  <xsl:attribute name="hres" >1920</xsl:attribute>
	      <xsl:attribute name="vres" >1080</xsl:attribute>
	</xsl:template>
</xsl:stylesheet>
 