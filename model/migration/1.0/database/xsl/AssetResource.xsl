<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
	<!--_common.xsl : include xsl template -->
    <xsl:import href="_common.xsl"/>
   
	<!--  define variables  -->
    <xsl:variable name="Ver">1.5</xsl:variable>
    <xsl:variable name="Rev">1</xsl:variable>
	 
 	<!-- call add attr on root -->
    <xsl:template mode="add-atts" match="/AssetResource">
        <!-- call add common attr - call-template addAttrs_rootEL to set serviceVer-->
		<xsl:call-template name="addAttrs_rootEL"/> 
    </xsl:template>


    <!--
		DELETE ELEMENTS AssetResource ( were present in 'objectXml_1.0' but
		removed from 'objectXml_1.3')
	-->

    <xsl:template match="/AssetResource/Contact/Reachable"/>
    <xsl:template match="/AssetResource/Info/DeviceNID"/>


    <!--
		Add ELEMENTS AssetResource ( were absent in 'objectXml_1.0' but added
		to 'objectXml_1.3')
	-->

    <!-- 
          /AssetResource/Contact/Presence
    -->

    <xsl:template mode="append" match="/AssetResource/Contact">
        <xsl:choose>
            <xsl:when test="Presence">
            </xsl:when>
            <xsl:otherwise>
                <Presence/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>