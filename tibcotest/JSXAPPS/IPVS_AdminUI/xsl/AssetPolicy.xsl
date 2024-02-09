<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="assetTemplate">
        <xsl:with-param name="asset_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="assetTemplate">
      <xsl:param name="asset_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$asset_el"/>
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
        <xsl:attribute name="jsxPolicyAssetXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAssetXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyAssetXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxAssetPolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxAssetPolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxAssetPolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxAssetPolicyTypeAllow">
            <xsl:value-of select="$record_el/allow"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleObject='MediaStreamSrcPortResource' and $record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='ViewFrom'">
                <xsl:attribute name="jsxAssetAdminSrcPortAccess">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetAdminSrcPortAccess">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleObject='MediaStreamDstPortResource' and $record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='StreamTo'">
                <xsl:attribute name="jsxAssetAdminDstPortAccess">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetAdminDstPortAccess">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='Add'">
                <xsl:attribute name="jsxAssetPolicyRoleAdd">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyRoleAdd">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='Modify'">
                <xsl:attribute name="jsxAssetPolicyRoleModify">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyRoleModify">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='Delete'">
                <xsl:attribute name="jsxAssetPolicyRoleDelete">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyRoleDelete">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='Read'">
                <xsl:attribute name="jsxAssetPolicyRoleView">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyRoleView">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/AssetAdminRoleSet/AssetAdminObjectRole/RoleAction='Start'">
                <xsl:attribute name="jsxAssetPolicyRoleStart">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxAssetPolicyRoleStart">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>