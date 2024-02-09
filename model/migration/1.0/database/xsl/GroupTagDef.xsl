<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
	<!--_common.xsl : include xsl template -->
    <xsl:import href="_common.xsl"/>
   
	<!--  define variables  -->
    <xsl:variable name="Ver">1.2</xsl:variable>
    <xsl:variable name="Rev">1</xsl:variable>
	 
 	<!-- call add attr on root -->
    <xsl:template mode="add-atts" match="/GroupTagDef">
        <!-- call add common attr - call-template addAttrs_rootEL to set serviceVer-->
		<xsl:call-template name="addAttrs_rootEL"/> 
    </xsl:template>
   
	<!--NOTE: NOTHING MUCH CHANGED -->
	
	
</xsl:stylesheet>