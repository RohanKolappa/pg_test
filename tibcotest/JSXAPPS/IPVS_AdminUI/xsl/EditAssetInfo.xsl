<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="AssetResource">
        <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="serviceVer">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>      
        <xsl:attribute name="Ver">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>   
        <xsl:attribute name="Rev">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute> 
        <xsl:attribute name="parentNID">
            <xsl:value-of select="$record_el/@parentNID"/>
        </xsl:attribute>
        <xsl:attribute name="Persistence">
            <xsl:value-of select="$record_el/@Persistence"/>
        </xsl:attribute> 
        <xsl:choose>
          <xsl:when test="$record_el/Contact/Presence='unmanaged'">
                <xsl:attribute name="Presence">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="Presence">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="OwnerUserJID">
            <xsl:value-of select="$record_el/Contact/OwnerUserJID"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/State/StateFlag='Offline'">
                <xsl:attribute name="StateFlag">Offline</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Ready'">
                <xsl:attribute name="StateFlag">Ready</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='NotReady'">
                <xsl:attribute name="StateFlag">NotReady</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Idle'">
                <xsl:attribute name="StateFlag">Idle</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Reserved'">
                <xsl:attribute name="StateFlag">Reserved</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Busy'">
                <xsl:attribute name="StateFlag">Busy</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Unmanaged'">
                <xsl:attribute name="StateFlag">Unmanaged</xsl:attribute>
          </xsl:when>
          <xsl:when test="$record_el/State/StateFlag='Template'">
                <xsl:attribute name="StateFlag">Template</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="StateFlag">Error</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="StateFlag">
            <xsl:value-of select="$record_el/State/StateFlag"/>
        </xsl:attribute>
        <xsl:attribute name="StateMsg">
            <xsl:value-of select="$record_el/State/StateMsg"/>
        </xsl:attribute> 
        <xsl:attribute name="TemplateAssetResourceNID">
            <xsl:value-of select="$record_el/TemplateAssetResourceNID"/>
        </xsl:attribute> 
        <xsl:attribute name="DeviceName">
            <xsl:value-of select="$record_el/Info/Title"/>
        </xsl:attribute> 
        <xsl:attribute name="DeviceType">
            <xsl:value-of select="$record_el/Info/Type"/>
        </xsl:attribute> 
        <xsl:attribute name="DeviceDescription">
            <xsl:value-of select="$record_el/Info/Description"/>
        </xsl:attribute>
        <xsl:choose>
            <xsl:when test="$record_el/Info/AssetPrivateKey/KeyScheme='MACADDRESS'">
                <xsl:attribute name="KeyScheme"></xsl:attribute>
            </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="KeyScheme">
                    <xsl:value-of select="$record_el/Info/AssetPrivateKey/KeyScheme"/>
                </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="$record_el/Info/AssetPrivateKey/KeyData='00:00:00:00:00:00'">
                <xsl:attribute name="KeyData"></xsl:attribute>
            </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="KeyData">
                    <xsl:value-of select="$record_el/Info/AssetPrivateKey/KeyData"/>
                </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="AssetPrivateData">
            <xsl:value-of select="$record_el/Info/AssetPrivateData"/>
        </xsl:attribute> 
     </xsl:template>
</xsl:stylesheet>