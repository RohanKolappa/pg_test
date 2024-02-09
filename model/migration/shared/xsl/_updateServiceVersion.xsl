<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="_dbtemplate.xsl"/>

    <xsl:param name="aftermigration_serviceVersion"/>
 
    <!-- if exists serviceVer -->
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer"><xsl:value-of select="$aftermigration_serviceVersion"></xsl:value-of> </xsl:attribute>
    </xsl:template>

</xsl:stylesheet>