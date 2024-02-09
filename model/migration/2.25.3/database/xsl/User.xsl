<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
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

	<xsl:template match="/User/Username">
		<xsl:choose>
			<xsl:when test="contains(. ,' ')">
				<Username><xsl:call-template name="ipvs-remove-text">
						<xsl:with-param name="text" select="." />
						<xsl:with-param name="replace" select="' '" />
					</xsl:call-template></Username>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="." />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="/User/Name">
		<xsl:choose>
			<xsl:when test="contains(. ,' ')">
				<Name><xsl:call-template name="ipvs-remove-text">
						<xsl:with-param name="text" select="." />
						<xsl:with-param name="replace" select="' '" />
					</xsl:call-template></Name>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="." />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="ipvs-remove-text">
		<xsl:param name="text" />
		<xsl:param name="replace" />
		<xsl:choose>
			<xsl:when test="contains($text, $replace)">
				<xsl:value-of select="substring-before($text, $replace)" />
				<xsl:call-template name="ipvs-remove-text">
					<xsl:with-param name="text"
						select="substring-after($text, $replace)" />
					<xsl:with-param name="replace" select="$replace" />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$text" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>

