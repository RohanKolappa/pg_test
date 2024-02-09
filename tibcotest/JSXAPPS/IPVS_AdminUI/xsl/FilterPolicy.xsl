<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="filterTemplate">
        <xsl:with-param name="filter_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="filterTemplate">
      <xsl:param name="filter_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$filter_el"/>
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
        <xsl:attribute name="jsxTagObjectXMLVersion">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>
        <xsl:attribute name="jsxTagObjectXMLRevision">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute>
        <xsl:attribute name="jsxTagObjectXMLServiceVersion">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPolicyFilterName">
            <xsl:value-of select="$record_el/Name"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='AssetResource'">
                <xsl:attribute name="jsxPolicyFilterTypeAsset">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeAsset">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='User'">
                <xsl:attribute name="jsxPolicyFilterTypeUsers">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeUsers">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='MediaStreamProfile'">
                <xsl:attribute name="jsxPolicyFilterTypeProfile">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeProfile">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='MediaStreamPortResource' or $record_el/TagObjectTypeList/TagObjectType='MediaStorePortResource'">
                <xsl:attribute name="jsxPolicyFilterTypeResource">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeResource">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='MediaStreamFileResource'  or $record_el/TagObjectTypeList/TagObjectType='MediaStreamDirResource'">
                <xsl:attribute name="jsxPolicyFilterTypeMedia">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeMedia">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/TagObjectTypeList/TagObjectType='MediaRoom'">
                <xsl:attribute name="jsxPolicyFilterTypeMediaRoom">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxPolicyFilterTypeMediaRoom">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>