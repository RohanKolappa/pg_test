
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="/">
		<MediaStoreAdminPolicyRule NID="" Rev="1" Ver="1.3" parentNID="default.mediastoreadminpolicyrulelist" serviceVer="">
			<xsl:variable name="assetnid">
				<xsl:value-of select="/MediaStoreAdminPolicyRule/@NID" />
			</xsl:variable>
			 <xsl:attribute name="NID">default.mediastoreadminpolicyrulelist.msaprl_<xsl:value-of select="substring-after($assetnid,'msaprl_')"/>_1</xsl:attribute>
			<rulename>
				<xsl:value-of select="/MediaStoreAdminPolicyRule/rulename" />
			</rulename>
			<order>
				<xsl:value-of select="/MediaStoreAdminPolicyRule/order" />
			</order>
			<valid>
				<xsl:value-of select="/MediaStoreAdminPolicyRule/valid" />
			</valid>
			<allow>
				<xsl:value-of select="/MediaStoreAdminPolicyRule/allow" />
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
				<xsl:for-each select="/MediaStoreAdminPolicyRule/ServiceOperatorFilterSet/ServiceOperatorFilter">
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
			<MediaStoreAdminRoleSet>
				<MediaStoreAdminObjectRole>
					<RoleObject>MediaStreamFileResource</RoleObject>
					<RoleAction>Read</RoleAction>
				</MediaStoreAdminObjectRole>
				<MediaStoreAdminObjectRole>
					<RoleObject>MediaStreamFileResource</RoleObject>
					<RoleAction>Playback</RoleAction>
				</MediaStoreAdminObjectRole>
				<MediaStoreAdminObjectRole>
					<RoleObject>MediaStreamDirResource</RoleObject>
					<RoleAction>Read</RoleAction>
				</MediaStoreAdminObjectRole>
			</MediaStoreAdminRoleSet>
			<MediaStoreAdminObjectFilterSet>
				<xsl:for-each select="/MediaStoreAdminPolicyRule/MediaStoreAdminObjectFilterSet/MediaStoreAdminObjectFilterNID">
					<MediaStoreAdminObjectFilterNID>
						<xsl:value-of select="." />
					</MediaStoreAdminObjectFilterNID>
				</xsl:for-each>
			</MediaStoreAdminObjectFilterSet>
			<RuleKeyTagFilterSet>
				<xsl:for-each select="/MediaStoreAdminPolicyRule/RuleKeyTagFilterSet/RuleKeyTagFilter">
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
		</MediaStoreAdminPolicyRule>
	</xsl:template>
</xsl:stylesheet>