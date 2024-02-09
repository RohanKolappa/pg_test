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
	<xsl:template match="*/order">
		<xsl:choose>
			<xsl:when test="name()='order'">
				<xsl:element name="order">1</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy></xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="*/UserFilterNID">
		<xsl:choose>
			<xsl:when test="name()='UserFilterNID'">
				<xsl:element name="UserFilterNID"></xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy></xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="*/ResourceFilterNID">
		<xsl:choose>
			<xsl:when test="name()='ResourceFilterNID'">
				<xsl:element name="ResourceFilterNID"></xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy></xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>