<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 <xsl:output method="xml" indent="yes"/>
 <xsl:param name='list_name'/>	
 <xsl:template match="/">
	 <xsl:apply-templates match="//*[exist_xmldb]"/>
 </xsl:template> 
 <xsl:template match="//*[exist_xmldb]"><xsl:choose><xsl:when test="local-name()=$list_name"> <xsl:copy-of select='exist_xmldb/*'/></xsl:when><xsl:otherwise></xsl:otherwise></xsl:choose> </xsl:template> 
</xsl:stylesheet>
