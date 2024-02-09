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

	<xsl:template match="//AssetAdminObjectRole">
		<xsl:choose>
			<xsl:when test="./RoleObject='MediaStreamPortResource'">
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamSrcPortResource</RoleObject>
					<xsl:call-template name="RoleActionchangeViewToRead">
						<xsl:with-param name="roleActionValue" select="./RoleAction" />
					</xsl:call-template>
				</AssetAdminObjectRole>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamDstPortResource</RoleObject>
					<xsl:call-template name="RoleActionchangeViewToRead">
						<xsl:with-param name="roleActionValue" select="./RoleAction" />
					</xsl:call-template>
				</AssetAdminObjectRole>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamRelayPortResource</RoleObject>
					<xsl:call-template name="RoleActionchangeViewToRead">
						<xsl:with-param name="roleActionValue" select="./RoleAction" />
					</xsl:call-template>
				</AssetAdminObjectRole>
			</xsl:when>
			<xsl:otherwise>
				<AssetAdminObjectRole>
					<xsl:copy-of select="./RoleObject" />
					<xsl:call-template name="RoleActionchangeViewToRead">
						<xsl:with-param name="roleActionValue" select="./RoleAction" />
					</xsl:call-template>
				</AssetAdminObjectRole>
			</xsl:otherwise>
		</xsl:choose>
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
	
	<xsl:variable name="noOfRoleObjects">
	  <xsl:value-of select="count(//RoleObject)"></xsl:value-of>
	</xsl:variable>
	
	<xsl:variable name="noOfSessionConfigRoleObjects">
	    <xsl:value-of select="count(//RoleObject[.='SessionConfig'])"></xsl:value-of>
	</xsl:variable>
	<xsl:template match="//AssetAdminObjectRole[RoleObject='SessionConfig']">
	  
	 <xsl:choose>
	     <xsl:when test="$noOfRoleObjects=$noOfSessionConfigRoleObjects"><xsl:copy-of select="."/></xsl:when>
	     <xsl:otherwise/>
	 </xsl:choose>
	</xsl:template>
	<xsl:template match="//AssetAdminObjectRole[RoleObject='CollaborationConfig']" />
	<xsl:template match="//AssetAdminRoleSet" mode='insert'>
	    <xsl:choose>
	       <xsl:when test=".//RoleObject='MediaStreamProfile'">    
	       	 <xsl:for-each select=".//AssetAdminObjectRole[RoleObject='MediaStreamProfile']">				
				<AssetAdminObjectRole>
					<RoleObject>DisplayProfile</RoleObject>
					<xsl:call-template name="RoleActionchangeViewToRead">
						<xsl:with-param name="roleActionValue" select="./RoleAction" />
					</xsl:call-template>
				</AssetAdminObjectRole>
				</xsl:for-each>
		    </xsl:when>	
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>
 
 