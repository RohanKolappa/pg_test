<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="mediaRoomTemplate">
        <xsl:with-param name="mediaRoom_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="mediaRoomTemplate">
      <xsl:param name="mediaRoom_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$mediaRoom_el"/>
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
        <xsl:attribute name="jsxMediaRoomPolicyXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxMediaRoomPolicyXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxMediaRoomPolicyXMLServiceVer">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxCollabPolicyName">
            <xsl:value-of select="$record_el/rulename"/>
        </xsl:attribute>
        <xsl:attribute name="jsxCollabPolicyOrder">
            <xsl:value-of select="$record_el/order"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/valid='true'">
                <xsl:attribute name="jsxCollabPolicyActive">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxCollabPolicyActive">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxCollabPolicyTypeAllow">
            <xsl:value-of select="$record_el/allow"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/ExternalCondition/XAuth/RuleKey='MEDIAROOM_RULE_CONFCREATE_USER'">
                <xsl:attribute name="jsxXAuthUseServiceOperator">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxXAuthUseServiceOperator">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/ExternalCondition/XAuth/RuleKey='MEDIAROOM_RULE_STREAM_SRC_RESOURCE'">
                <xsl:attribute name="jsxXAuthUseSource">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxXAuthUseSource">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/ExternalCondition/XAuth/RuleKey='MEDIAROOM_RULE_STREAM_DST_RESOURCE'">
                <xsl:attribute name="jsxXAuthUseDest">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxXAuthUseDest">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaRoomRoleSet/MediaRoomObjectRole/RoleAction='PVR'">
                <xsl:attribute name="jsxCollabPolicyRolePVR">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxCollabPolicyRolePVR">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaRoomRoleSet/MediaRoomObjectRole/RoleAction='Record'">
                <xsl:attribute name="jsxCollabPolicyRoleRecord">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxCollabPolicyRoleRecord">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaRoomRoleSet/MediaRoomObjectRole/RoleAction='View'">
                <xsl:attribute name="jsxCollabPolicyRoleView">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxCollabPolicyRoleView">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/MediaRoomRoleSet/MediaRoomObjectRole/RoleAction='Playback'">
                <xsl:attribute name="jsxCollabPolicyRoleViewPlayback">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxCollabPolicyRoleViewPlayback">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>