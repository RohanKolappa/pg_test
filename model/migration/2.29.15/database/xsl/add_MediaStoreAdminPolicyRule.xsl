<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="/">
		<MediaStoreAdminPolicyRule NID="" Rev="1" Ver="1.3" parentNID="default.mediastoreadminpolicyrulelist" serviceVer="2.29.16">
			<xsl:variable name="mediaroompolicynid">
				<xsl:value-of select="/MediaRoomPolicyRule/@NID" />
			</xsl:variable>
			 <xsl:attribute name="NID">default.mediastoreadminpolicyrulelist.msaprl_<xsl:value-of select="substring-after($mediaroompolicynid,'mrprl_')"/>_2_29_15</xsl:attribute>
			<rulename>
				<xsl:value-of select="/MediaRoomPolicyRule/rulename" />
			</rulename>
			<order>
				<xsl:value-of select="/MediaRoomPolicyRule/order" />
			</order>
			<valid>
				<xsl:value-of select="/MediaRoomPolicyRule/valid" />
			</valid>
			<allow>
				<xsl:value-of select="/MediaRoomPolicyRule/allow" />
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
				<xsl:for-each select="/MediaRoomPolicyRule/ServiceOperatorFilterSet/ServiceOperatorFilter">
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
					<RoleObject>MediaStreamDirResource</RoleObject>
					<RoleAction>PVR</RoleAction>
				</MediaStoreAdminObjectRole>
				<MediaStoreAdminObjectRole>
					<RoleObject>MediaStreamDirResource</RoleObject>
					<RoleAction>Read</RoleAction>
				</MediaStoreAdminObjectRole>
			</MediaStoreAdminRoleSet>
			<MediaStoreAdminObjectFilterSet>
				<xsl:for-each select="/MediaRoomPolicyRule/DstFilterSet/DstFilter">
					<MediaStoreAdminObjectFilterNID>
						<xsl:value-of select="./MediaStreamResourceFilterNID" />
					</MediaStoreAdminObjectFilterNID>
				</xsl:for-each>
			</MediaStoreAdminObjectFilterSet>
			<RuleKeyTagFilterSet>
				<xsl:for-each select="/MediaRoomPolicyRule/RuleKeyTagFilterSet/RuleKeyTagFilter">
					<xsl:choose>
						<xsl:when
							test="(./*/RuleKey='MEDIAROOM_RULE_CONFCREATE_USER' or /*/RuleKey='MEDIAROOM_RULE_CONFSTART_USER' or ./*/RuleKey='MEDIAROOM_RULE_STREAM_DST_RESOURCE') and ./RuleKeyLHTagName/RuleKey!='MEDIAROOM_RULE_STREAM_SRC_RESOURCE' and ./RuleKeyRHTagName/RuleKey!='MEDIAROOM_RULE_STREAM_SRC_RESOURCE'">
							<RuleKeyTagFilter>
								<RuleKeyLHTagName>
									<RuleKey>
									<xsl:choose>
										<xsl:when test="(./RuleKeyLHTagName/RuleKey='MEDIAROOM_RULE_CONFCREATE_USER' or ./RuleKeyLHTagName/RuleKey='MEDIAROOM_RULE_CONFSTART_USER')" >
											<xsl:text>SERVICE_RULE_OPER_USER</xsl:text>
										</xsl:when>
									</xsl:choose>
									<xsl:choose>
										<xsl:when test="(./RuleKeyLHTagName/RuleKey='MEDIAROOM_RULE_STREAM_DST_RESOURCE')" >
											<xsl:text>SERVICE_RULE_TARGET_OBJECT</xsl:text>
										</xsl:when>
									</xsl:choose>
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
									<xsl:choose>
										<xsl:when test="(./RuleKeyRHTagName/RuleKey='MEDIAROOM_RULE_CONFCREATE_USER' or ./RuleKeyRHTagName/RuleKey='MEDIAROOM_RULE_CONFSTART_USER')" >
										<xsl:text>SERVICE_RULE_OPER_USER</xsl:text>
									   </xsl:when>
									   </xsl:choose>
									   <xsl:choose>
									   <xsl:when test="(./RuleKeyRHTagName/RuleKey='MEDIAROOM_RULE_STREAM_DST_RESOURCE')" >
									   <xsl:text>SERVICE_RULE_TARGET_OBJECT</xsl:text>
									   </xsl:when>
									   </xsl:choose>
									</RuleKey>
									<TagName>
										<RuleKey>
											<xsl:value-of select="./RuleKeyRHTagName/TagName" />
										</RuleKey>
									</TagName>
								</RuleKeyRHTagName>
							</RuleKeyTagFilter>
						</xsl:when>
					</xsl:choose>
				</xsl:for-each>
			</RuleKeyTagFilterSet>
		</MediaStoreAdminPolicyRule>
	</xsl:template>
</xsl:stylesheet>
