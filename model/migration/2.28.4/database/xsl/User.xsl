
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.28.5</xsl:attribute>
    </xsl:template>
    <xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.5</xsl:attribute>
    </xsl:template>

    <xsl:template mode="after" match="//Username">
    	<ExternalAuthentication/>
    </xsl:template>

</xsl:stylesheet>