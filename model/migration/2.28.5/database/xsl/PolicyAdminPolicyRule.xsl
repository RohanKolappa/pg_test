<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.28.6</xsl:attribute>
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

	<xsl:template match="//PolicyAdminRoleSet" mode='insert'>
		<PolicyAdminObjectRole>
			<RoleObject>Report</RoleObject>
			<RoleAction>Add</RoleAction>
		</PolicyAdminObjectRole>
		<PolicyAdminObjectRole>
			<RoleObject>Report</RoleObject>
			<RoleAction>Delete</RoleAction>
		</PolicyAdminObjectRole>
		<PolicyAdminObjectRole>
			<RoleObject>Report</RoleObject>
			<RoleAction>Modify</RoleAction>
		</PolicyAdminObjectRole>
		<PolicyAdminObjectRole>
			<RoleObject>Report</RoleObject>
			<RoleAction>Read</RoleAction>
		</PolicyAdminObjectRole>
	</xsl:template>

	<xsl:template match="//PolicyAdminObjectRole[RoleObject='System' and RoleAction='Report']" />
</xsl:stylesheet>

 
 
