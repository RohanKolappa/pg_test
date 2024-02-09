<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.28.3</xsl:attribute>
	</xsl:template>
	<!-- Here's the boilerplate code -->
	<!-- By default, copy all nodes unchanged -->
	<xsl:template match="@* | node()">
		<xsl:apply-templates mode="before" select="." />
		<xsl:copy>
			<xsl:apply-templates select="@*" />
			<xsl:apply-templates mode="add-atts" select="." />
			<xsl:apply-templates mode="insert" select="." />
			<xsl:apply-templates />
			<xsl:apply-templates mode="append" select="." />
		</xsl:copy>
		<xsl:apply-templates mode="after" select="." />
	</xsl:template>

	<!-- By default, don't add anything -->
	<xsl:template mode="add-atts" match="*" />
	<xsl:template mode="insert" match="*" />
	<xsl:template mode="append" match="*" />
	<xsl:template mode="before" match="@* | node()" />
	<xsl:template mode="after" match="@* | node()" />


	<xsl:template match="//RoleAction">
		<xsl:call-template name="RoleActionchangeViewToRead">
			<xsl:with-param name="roleActionValue" select="." />
		</xsl:call-template>
	</xsl:template>
	<xsl:template name="RoleActionchangeViewToRead">
		<xsl:param name="roleActionValue" />
		<xsl:choose>
			<xsl:when test="$roleActionValue='View'">
				<RoleAction>Read</RoleAction>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="$roleActionValue"></xsl:copy-of>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>

 
 