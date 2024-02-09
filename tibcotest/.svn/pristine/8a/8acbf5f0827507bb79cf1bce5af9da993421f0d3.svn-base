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
        <xsl:attribute name="DirtyBitblkAssetStreamSettings">
            <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
        </xsl:attribute> 
        <xsl:attribute name="MulticastAddrRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="MulticastAddrRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/To"/>
        </xsl:attribute>
        <xsl:attribute name="MulticastTTL">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MulticastTTL"/>
        </xsl:attribute>
        <xsl:attribute name="CtrlPortRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/V2DStreamsTxConfig/TxCtrlPortRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="CtrlPortRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/V2DStreamsTxConfig/TxCtrlPortRange/To"/>
        </xsl:attribute>
        <xsl:attribute name="AVPortRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/V2DStreamsRxConfig/RxMediaPortRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="AVPortRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/V2DStreamsRxConfig/RxMediaPortRange/To"/>
        </xsl:attribute>
        <xsl:attribute name="MPEGTSAVPortRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/MPEGTSStreamsRxConfig/RxMediaPortRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="MPEGTSAVPortRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/MPEGTSStreamsRxConfig/RxMediaPortRange/To"/>
        </xsl:attribute>
        <xsl:attribute name="RTPAVPortRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/RTPStreamsRxConfig/RxMediaPortRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="RTPAVPortRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/RTPStreamsRxConfig/RxMediaPortRange/To"/>
        </xsl:attribute>
        <xsl:attribute name="UDPAVPortRangeFrom">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/UDPStreamsRxConfig/RxMediaPortRange/From"/>
        </xsl:attribute>
        <xsl:attribute name="UDPAVPortRangeTo">
            <xsl:value-of select="$record_el/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/UDPStreamsRxConfig/RxMediaPortRange/To"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>