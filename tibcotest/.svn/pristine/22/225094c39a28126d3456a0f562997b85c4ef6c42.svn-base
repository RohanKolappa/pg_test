<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="deviceTemplate">
        <xsl:with-param name="device_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="deviceTemplate">
      <xsl:param name="device_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$device_el"/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>
    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkAssetDNSSettings">
            <xsl:value-of select="@DirtyBitblkAssetDNSSettings"/>
        </xsl:attribute> 
        <xsl:attribute name="V2DHostname">
            <xsl:value-of select="$record_el/DeviceConfig/NetworkElementsConfig/DNSClientConfig/HostName"/>
        </xsl:attribute>
         <xsl:attribute name="V2DDomainName">
            <xsl:value-of select="$record_el/DeviceConfig/NetworkElementsConfig/DNSClientConfig/DNSDomain"/>
        </xsl:attribute>
         <xsl:attribute name="V2DDNS">
            <xsl:value-of select="$record_el/DeviceConfig/NetworkElementsConfig/DNSClientConfig/DNSServerIP"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkNTPInfo">
            <xsl:value-of select="@DirtyBitblkNTPInfo"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/SystemElementsConfig/SystemClockConfig/EnableNTPClient='true'">
                <xsl:attribute name="EnableNTPClient">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableNTPClient">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="NTPServerIP">
            <xsl:value-of select="$record_el/DeviceConfig/SystemElementsConfig/SystemClockConfig/NTPServerIP"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkNTPServer">
            <xsl:value-of select="@DirtyBitblkNTPServer"/>
        </xsl:attribute> 
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/ServiceElementsConfig/NTPServiceConfig/EnableNTPServer='true'">
                <xsl:attribute name="EnableServerNTPServer">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableServerNTPServer">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="NTPServerServerIP">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/NTPServiceConfig/NTPServerIP"/>
        </xsl:attribute>

     </xsl:template>
</xsl:stylesheet>