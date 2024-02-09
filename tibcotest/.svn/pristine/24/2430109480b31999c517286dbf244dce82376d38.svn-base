<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="policyAdminTemplate">
        <xsl:with-param name="policyAdmin_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="policyAdminTemplate">
      <xsl:param name="policyAdmin_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$policyAdmin_el"/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>
    
    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="parentNID">
            <xsl:value-of select="$record_el/@parentNID"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAdminPolicyXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAdminPolicyXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAdminPolicyXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAdminPolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAdminPolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxPolicyAdminPolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyAdminPolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxPolicyAdminPolicyTypeAllow">
            <xsl:value-of select="$record_el/allow"/>
        </xsl:attribute>
    </xsl:template>
</xsl:stylesheet>