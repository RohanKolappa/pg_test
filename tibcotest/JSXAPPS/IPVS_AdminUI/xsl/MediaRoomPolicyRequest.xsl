<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <MediaRoomAdminPolicyRule>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@jsxMediaRoomPolicyXMLRevision"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@jsxMediaRoomPolicyXMLVersion"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@jsxMediaRoomPolicyXMLServiceVer"/>
          </xsl:attribute>
          <rulename>
              <xsl:value-of select="@jsxCollabPolicyName"/>
          </rulename> 
          <order>
              <xsl:value-of select="@jsxCollabPolicyOrder"/>
          </order>
          <valid>
            <xsl:choose>
              <xsl:when test="@jsxCollabPolicyActive='true' or @jsxCollabPolicyActive='1'">true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </valid>
          <allow>
            <xsl:choose>
              <xsl:when test="@jsxCollabPolicyTypeAllow='true' or @jsxCollabPolicyTypeAllow='1'" >true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </allow>
         <ExternalCondition>
             <TimeRange>
                  <From /> 
                  <To /> 
              </TimeRange>
             <XAuth>
                  <RuleKey> 
                    <xsl:choose>
                      <xsl:when test="@jsxXAuthUseServiceOperator='1'">MEDIAROOM_RULE_CONFCREATE_USER</xsl:when>
                      <xsl:when test="@jsxXAuthUseServiceOperator='0'">false</xsl:when>
                    </xsl:choose>              
                  </RuleKey> 
                  <RuleKey> 
                    <xsl:choose>
                      <xsl:when test="@jsxXAuthUseSource='1'">MEDIAROOM_RULE_STREAM_SRC_RESOURCE</xsl:when>
                      <xsl:when test="@jsxXAuthUseSource='0'">false</xsl:when>
                    </xsl:choose>              
                  </RuleKey> 
                  <RuleKey> 
                    <xsl:choose>
                      <xsl:when test="@jsxXAuthUseDest='1'">MEDIAROOM_RULE_STREAM_DST_RESOURCE</xsl:when>
                      <xsl:when test="@jsxXAuthUseDest='0'">false</xsl:when>
                    </xsl:choose>              
                  </RuleKey> 
                  <ServiceXAuthOperatorFilterSet /> 
              </XAuth>
              <StateFlag>
                  <FlagName /> 
                  <FlagValue /> 
              </StateFlag>
          </ExternalCondition>
          <ServiceOperatorFilterSet>
          </ServiceOperatorFilterSet>
          <MediaRoomRoleSet>
          </MediaRoomRoleSet>
          <MediaRoomObjectFilterSet>
          </MediaRoomObjectFilterSet>
          <SrcFilterSet>
          </SrcFilterSet>
          <DstFilterSet>
          </DstFilterSet>
          <MediaRoomStreamFilterSpec>
            <MediaRoomStreamFilterListLookupMode /> 
            <MediaRoomStreamFilterSet /> 
          </MediaRoomStreamFilterSpec>
          <RuleKeyTagFilterSet>
          </RuleKeyTagFilterSet>
        </MediaRoomAdminPolicyRule>
    </xsl:template>

 </xsl:stylesheet>
