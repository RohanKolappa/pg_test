<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <AssetAdminPolicyRule>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@jsxPolicyAssetXMLRevision"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@jsxPolicyAssetXMLVersion"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@jsxPolicyAssetXMLServiceVersion"/>
          </xsl:attribute>
          <rulename>
              <xsl:value-of select="@jsxAssetPolicyName"/>
          </rulename> 
          <order>
              <xsl:value-of select="@jsxAssetPolicyOrder"/>
          </order>
          <valid>
            <xsl:choose>
              <xsl:when test="@jsxAssetPolicyActive='true' or @jsxAssetPolicyActive='1'">true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </valid>
          <allow>
            <xsl:choose>
              <xsl:when test="@jsxAssetPolicyTypeAllow='true' or @jsxAssetPolicyTypeAllow='1'" >true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </allow>
         <ExternalCondition>
             <TimeRange>
                  <From /> 
                  <To /> 
              </TimeRange>
             <XAuth>
                  <RuleKey/> 
                  <ServiceXAuthOperatorFilterSet /> 
              </XAuth>
              <StateFlag>
                  <FlagName /> 
                  <FlagValue /> 
              </StateFlag>
          </ExternalCondition>
          <ServiceOperatorFilterSet>
          </ServiceOperatorFilterSet>
          <AssetAdminRoleSet>
          </AssetAdminRoleSet>
          <AssetAdminObjectFilterSet>
          </AssetAdminObjectFilterSet>
          <RuleKeyTagFilterSet>
          </RuleKeyTagFilterSet>
        </AssetAdminPolicyRule>
    </xsl:template>

 </xsl:stylesheet>
