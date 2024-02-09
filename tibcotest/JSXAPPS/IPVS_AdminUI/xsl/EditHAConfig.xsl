<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="Device">
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
        <xsl:attribute name="DirtyBitblkHAConfig">
            <xsl:value-of select="@DirtyBitblkHAConfig"/>
        </xsl:attribute>
        <xsl:attribute name="jsxHAServerEnable">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/HAServerEnable"/>
        </xsl:attribute>
      <xsl:attribute name="jsxIsPrimary">
        <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/Primary"/>
      </xsl:attribute>
        <xsl:attribute name="jsxPeerIPAddress">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/PeerIP"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPeerName">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/PeerName"/>
        </xsl:attribute>
        <xsl:attribute name="jsxEnableVIP">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/VirtualIPEnable"/>
        </xsl:attribute>
        <xsl:attribute name="jsxVirtualIPAddress">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/VirtualIP"/>
        </xsl:attribute>
        <xsl:attribute name="jsxVirtualIPAddress2">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/VirtualIP2"/>
        </xsl:attribute>      
        <xsl:attribute name="jsxHAInterface">
          <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/HAServiceConfig/HAServer/Interface"/>
        </xsl:attribute>
    </xsl:template>
</xsl:stylesheet>