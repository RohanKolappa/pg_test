
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="/">
		<AssetAdminPolicyRule NID="" Rev="1" Ver="1.3" parentNID="default.assetadminpolicyrulelist" serviceVer="2.28.1">
			<xsl:variable name="assetnid">
				<xsl:value-of select="/AssetAdminPolicyRule/@NID" />
			</xsl:variable>
			 <xsl:attribute name="NID">default.assetadminpolicyrulelist.aaprl_<xsl:value-of select="substring-after($assetnid,'aaprl_')"/>_1</xsl:attribute>
			<rulename>
				<xsl:value-of select="/AssetAdminPolicyRule/rulename" />
			</rulename>
			<order>
				<xsl:value-of select="/AssetAdminPolicyRule/order" />
			</order>
			<valid>
				<xsl:value-of select="/AssetAdminPolicyRule/valid" />
			</valid>
			<allow>
				<xsl:value-of select="/AssetAdminPolicyRule/allow" />
			</allow>
			<ExternalCondition>
				<TimeRange>
					<From />
					<To />
				</TimeRange>
				<XAuth>
					<RuleKey />
					<ServiceXAuthOperatorFilterSet>
						<ServiceOperatorFilter>
							<UserFilterNID />
							<ResourceFilterNID />
						</ServiceOperatorFilter>
					</ServiceXAuthOperatorFilterSet>
				</XAuth>
				<StateFlag>
					<FlagName />
					<FlagValue />
				</StateFlag>
			</ExternalCondition>
			<ServiceOperatorFilterSet>
				<xsl:for-each select="/AssetAdminPolicyRule/ServiceOperatorFilterSet/ServiceOperatorFilter">
					<ServiceOperatorFilter>
						<UserFilterNID>
							<xsl:value-of select="./UserFilterNID" />
						</UserFilterNID>
						<ResourceFilterNID>
							<xsl:value-of select="./ResourceFilterNID" />
						</ResourceFilterNID>
					</ServiceOperatorFilter>
				</xsl:for-each>
			</ServiceOperatorFilterSet>
			<AssetAdminRoleSet>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamSrcPortResource</RoleObject>
					<RoleAction>Read</RoleAction>
				</AssetAdminObjectRole>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamSrcPortResource</RoleObject>
					<RoleAction>ViewFrom</RoleAction>
				</AssetAdminObjectRole>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamDstPortResource</RoleObject>
					<RoleAction>Read</RoleAction>
				</AssetAdminObjectRole>
				<AssetAdminObjectRole>
					<RoleObject>MediaStreamDstPortResource</RoleObject>
					<RoleAction>StreamTo</RoleAction>
				</AssetAdminObjectRole>
			</AssetAdminRoleSet>
			<AssetAdminObjectFilterSet>
				<xsl:for-each select="/AssetAdminPolicyRule/AssetAdminObjectFilterSet/AssetAdminObjectFilterNID">
					<AssetAdminObjectFilterNID>
						<xsl:value-of select="." />
					</AssetAdminObjectFilterNID>
				</xsl:for-each>
			</AssetAdminObjectFilterSet>
			<RuleKeyTagFilterSet>
				<xsl:for-each select="/AssetAdminPolicyRule/RuleKeyTagFilterSet/RuleKeyTagFilter">
					<RuleKeyTagFilter>
						<RuleKeyLHTagName>
							<RuleKey>
								<xsl:value-of select="./RuleKeyLHTagName/RuleKey" />
							</RuleKey>
							<TagName>
								<xsl:value-of select="./RuleKeyLHTagName/TagName" />
							</TagName>
						</RuleKeyLHTagName>
						<MatchOp>
							<xsl:value-of select="./MatchOp" />
						</MatchOp>
						<RuleKeyRHTagName>
							<RuleKey>
								<xsl:value-of select="./RuleKeyRHTagName/RuleKey" />
							</RuleKey>
							<TagName>
								<RuleKey>
									<xsl:value-of select="./RuleKeyRHTagName/TagName" />
								</RuleKey>
							</TagName>
						</RuleKeyRHTagName>
					</RuleKeyTagFilter>
				</xsl:for-each>
			</RuleKeyTagFilterSet>
		</AssetAdminPolicyRule>
	</xsl:template>
</xsl:stylesheet>