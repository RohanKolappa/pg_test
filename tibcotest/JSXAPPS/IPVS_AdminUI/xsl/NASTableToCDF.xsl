<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="NASConfig/NASTable">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="NASConfig/NASTable/NASEntry">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/@Dirty!='CLEAN'">
            <xsl:attribute name="jsxDirtyBit">
                <xsl:value-of select="$record_el/@Dirty"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxDirtyBit"></xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxListIndex">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxMediaStoreName">
            <xsl:value-of select="$record_el/MediaStore/Name"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/Enable='true'">
                <xsl:attribute name="jsxEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxServer">
            <xsl:value-of select="$record_el/Server"/>
        </xsl:attribute>
        <xsl:attribute name="jsxRemoteDir">
            <xsl:value-of select="$record_el/RemoteDirectory"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/ReadOnly='true'">
                <xsl:attribute name="jsxReadOnly">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxReadOnly">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxMediaStoreRootDirPath">
            <xsl:value-of select="$record_el/MediaStore/RootDirPath"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>