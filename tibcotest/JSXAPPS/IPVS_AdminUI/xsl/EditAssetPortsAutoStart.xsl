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
        <xsl:attribute name="DirtyBitblkVideoConfigCnt">
            <xsl:value-of select="@DirtyBitblkVideoConfigCnt"/>
        </xsl:attribute>
	    <xsl:choose>
          <xsl:when test="$record_el/InstanceList/Instance/@mode='AutoStart'">
                <xsl:attribute name="EnableAutoStart">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableAutoStart">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterKbmConfig/EnableKbm='true'">
                <xsl:attribute name="EnableKbm">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableKbm">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterKbmConfig/AllowKMSwitch='true'">
                <xsl:attribute name="AllowKMSwitch">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowKMSwitch">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/InstanceList/Instance/ProfileXML/V2DRecorderConfig/FFTracks='true'">
                <xsl:attribute name="FFTracks">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="FFTracks">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/InstanceList/Instance/ProfileXML/V2DStreamConfig/IsMulticast='true'">
                <xsl:attribute name="IsMulticast">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="IsMulticast">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="AVOption">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DStreamConfig/AVOption"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkVideoConfigCnt">
            <xsl:value-of select="@DirtyBitblkVideoConfigCnt"/>
        </xsl:attribute> 
        <xsl:attribute name="Bandwidth">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Bandwidth"/>
        </xsl:attribute>
        <xsl:attribute name="FrameRateDivider">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/FrameRateDivider"/>
        </xsl:attribute>
        <xsl:attribute name="BurstRatio">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/BurstRatio"/>
        </xsl:attribute>
        <xsl:attribute name="BurstDuration">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/BurstDuration"/>
        </xsl:attribute>
        <xsl:attribute name="CompressionLow">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Compression/Low"/>
        </xsl:attribute>
        <xsl:attribute name="CompressionHigh">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Compression/High"/>
        </xsl:attribute>
        <xsl:attribute name="RefreshMin">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Refresh/Min"/>
        </xsl:attribute>
        <xsl:attribute name="RefreshMax">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Refresh/Max"/>
        </xsl:attribute>
        <xsl:attribute name="SliceMin">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Slice/Min"/>
        </xsl:attribute>
        <xsl:attribute name="SliceMax">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/Slice/Max"/>
        </xsl:attribute>
        <xsl:attribute name="BlockThreshold">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/BlockThreshold"/>
        </xsl:attribute>
        <xsl:attribute name="ColorSampling">
            <xsl:value-of select="$record_el/InstanceList/Instance/ProfileXML/V2DEncoderConnectionParameterConfig/ColorSampling"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>