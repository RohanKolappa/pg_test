
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->



    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">1.4</xsl:attribute>
    </xsl:template>


    <!-- Add ELEMENTS MediaStreamDirResource ( were absent in 'objectXml_1.3' but added to  'objectXml_1.4') -->

    <!-- 
          /MediaStreamDirResource/Info/Properties/DiskFullThreshold
          /MediaStreamDirResource/Info/Properties/SizeUsed
    -->

    <xsl:template mode="append" match="/MediaStreamDirResource/Info/Properties">
        <xsl:choose>
            <xsl:when test="DiskFullThreshold">
            </xsl:when>
            <xsl:otherwise>
                <DiskFullThreshold>0</DiskFullThreshold>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="SizeUsed">
            </xsl:when>
            <xsl:otherwise>
                <SizeUsed>0</SizeUsed>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>