<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

	 <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.21</xsl:attribute>
    </xsl:template>

	<xsl:template mode="append" match="/ServiceResource">
		 <xsl:choose>
			<xsl:when test='HA'>
			</xsl:when>
			<xsl:otherwise>
				  <HA>
				      <Primary MSAssetResourceNID="" state=""/>
					  <Secondary MSAssetResourceNID="" state=""/>
				  </HA>
			</xsl:otherwise>
        </xsl:choose>            
		 <xsl:choose>
			<xsl:when test='AssetResourceNID'>
			</xsl:when>
			<xsl:otherwise>
				  <AssetResourceNID></AssetResourceNID>
			</xsl:otherwise>
        </xsl:choose>            

	</xsl:template>
</xsl:stylesheet>
 