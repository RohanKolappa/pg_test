<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <UserAdminPolicyRule>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@jsxUserPolicyXMLRevision"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@jsxUserPolicyXMLVersion"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@jsxUserPolicyXMLServiceVersion"/>
          </xsl:attribute>
          <rulename>
              <xsl:value-of select="@jsxUserPolicyName"/>
          </rulename> 
          <order>
              <xsl:value-of select="@jsxUserPolicyOrder"/>
          </order>
          <valid>
            <xsl:choose>
              <xsl:when test="@jsxUserPolicyActive='true' or @jsxUserPolicyActive='1'">true</xsl:when>
              <xsl:otherwise>false</xsl:otherwise>
            </xsl:choose>              
          </valid>
          <allow>
            <xsl:choose>
              <xsl:when test="@jsxUserPolicyTypeAllow='true' or @jsxUserPolicyTypeAllow='1'" >true</xsl:when>
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
          <UserAdminRoleSet>
          </UserAdminRoleSet>
          <UserAdminObjectFilterSet>
          </UserAdminObjectFilterSet>
          <RuleKeyTagFilterSet>
          </RuleKeyTagFilterSet>
        </UserAdminPolicyRule>
    </xsl:template>

 </xsl:stylesheet>
