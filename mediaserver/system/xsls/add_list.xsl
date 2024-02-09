<?xml version="1.0"?>
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" version="1.0" encoding="ISO-8859-1" indent="yes" />
<xsl:param name="filename"/>
<xsl:param name="cmdname"/>
<xsl:param name="tablename"/>
<xsl:template match="@*|node()"> 
	<xsl:copy> 
		<xsl:apply-templates select="@*|node()" /> 
	</xsl:copy> 
</xsl:template>
<xsl:template match="//RouteTableConfig">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_route_entry"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
<xsl:template match="//EthernetPortTableConfig">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_if_list"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
<xsl:template match="//HostTableConfig">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_host_list"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
<xsl:template match="//NATZoneTableConfig">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_natzone_list"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
<xsl:template match='//V2DCodecVideoLookupTable'>
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_lut_list"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
<xsl:template match="//V2DDisplayOverrideTable">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" /> 
		<xsl:choose>
			<xsl:when test='"add_override_list"=$cmdname'>
				<xsl:copy-of select="document($filename)/*"/>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:copy>
</xsl:template>
</xsl:transform>
