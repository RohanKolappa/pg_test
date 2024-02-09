<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="userAdminTemplate">
        <xsl:with-param name="userAdmin_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="userAdminTemplate">
      <xsl:param name="userAdmin_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$userAdmin_el"/>
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
        <xsl:attribute name="jsxUserPolicyXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxUserPolicyXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxUserPolicyXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxUserPolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxUserPolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxUserPolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxUserPolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxUserPolicyTypeAllow">
            <xsl:value-of select="$record_el/allow"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/UserAdminRoleSet/UserAdminObjectRole/RoleAction='Add'">
                <xsl:attribute name="jsxUserPolicyRoleAdd">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxUserPolicyRoleAdd">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/UserAdminRoleSet/UserAdminObjectRole/RoleAction='Modify'">
                <xsl:attribute name="jsxUserPolicyRoleModify">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxUserPolicyRoleModify">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/UserAdminRoleSet/UserAdminObjectRole/RoleAction='Delete'">
                <xsl:attribute name="jsxUserPolicyRoleDelete">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxUserPolicyRoleDelete">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/UserAdminRoleSet/UserAdminObjectRole/RoleAction='Read'">
                <xsl:attribute name="jsxUserPolicyRoleRead">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxUserPolicyRoleRead">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>