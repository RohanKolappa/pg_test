<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>

    <xsl:template match="/">
        <data jsxid="jsxroot">
	        <xsl:call-template name="getgrouptagvalues">
		        <xsl:with-param name="parent_el" select="//*/Groups"/>
	        </xsl:call-template> 
        </data>
    </xsl:template>
   <xsl:template match="*"/>


   <xsl:template name="getgrouptagvalues">
      <xsl:param name="parent_el"/>
      <xsl:for-each select="$parent_el//*">
	      <xsl:variable select="name()" name="tagName" />
          <xsl:if test="( not( $tagName=preceding-sibling::Tag ) ) ">
           <xsl:copy-of select="." />
          </xsl:if>
      </xsl:for-each>
   </xsl:template>
   

</xsl:stylesheet>
