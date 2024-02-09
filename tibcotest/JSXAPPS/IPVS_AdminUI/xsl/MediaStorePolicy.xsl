<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="mediaStoreTemplate">
        <xsl:with-param name="mediaStore_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="mediaStoreTemplate">
      <xsl:param name="mediaStore_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$mediaStore_el"/>
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
        <xsl:attribute name="jsxPolicyMediaStoreXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyMediaStoreXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyMediaStoreXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxMediaStorePolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxMediaStorePolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxMediaStorePolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStorePolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxMediaStorePolicyTypeAllow">
            <xsl:value-of select="$record_el/allow"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleObject='MediaStreamDirResource' and $record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleAction='Record'">
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleRecord">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleRecord">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleObject='MediaStreamDirResource' and $record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleAction='PVR'">
                <xsl:attribute name="jsxMediaStoreDirPolicyRolePVR">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreDirPolicyRolePVR">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[RoleObject='MediaStreamFileResource']/RoleAction='Modify'">
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleModify">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleModify">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[RoleObject='MediaStreamFileResource']/RoleAction='Delete'">
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleDelete">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleDelete">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleObject='MediaStreamFileResource' and $record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleAction='Export'">
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleExport">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreFilePolicyRoleExport">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[RoleObject='MediaStreamDirResource']/RoleAction='Modify'">
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleModify">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleModify">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole[RoleObject='MediaStreamDirResource']/RoleAction='Delete'">
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleDelete">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleDelete">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleObject='MediaStreamDirResource' and $record_el/MediaStoreAdminRoleSet/MediaStoreAdminObjectRole/RoleAction='Import'">
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleImport">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxMediaStoreDirPolicyRoleImport">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>