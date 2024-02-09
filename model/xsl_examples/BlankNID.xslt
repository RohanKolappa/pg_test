<?xml version="1.0"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	
	<xsl:template match="*">
		<xsl:copy>
			<xsl:apply-templates select="@*"></xsl:apply-templates>
			<xsl:apply-templates></xsl:apply-templates>
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="@*">
		<xsl:choose>
			<xsl:when test="name()='NID'">
				<xsl:attribute name="NID"></xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy></xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>