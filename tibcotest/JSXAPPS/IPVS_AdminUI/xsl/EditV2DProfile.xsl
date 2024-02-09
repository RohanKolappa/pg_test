<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="V2DMediaStreamProfile">
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
        <xsl:attribute name="StreamType">
            <xsl:value-of select="$record_el/StreamType"/>
        </xsl:attribute> 
        <xsl:attribute name="Priority">
            <xsl:value-of select="$record_el/Priority"/>
        </xsl:attribute> 
        <xsl:attribute name="Title">
            <xsl:value-of select="$record_el/Title"/>
        </xsl:attribute> 
        <xsl:attribute name="Description">
            <xsl:value-of select="$record_el/Description"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowPause='true'">
                <xsl:attribute name="AllowPause">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowPause">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowSkipback='true'">
                <xsl:attribute name="AllowSkipback">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowSkipback">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowSkipfwd='true'">
                <xsl:attribute name="AllowSkipfwd">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowSkipfwd">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:choose>
          <xsl:when test="$record_el/Info/V2DStreamConfig/IsMulticast='true'">
                <xsl:attribute name="IsMulticast">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="IsMulticast">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="EnableSRDTranscode">
            <xsl:value-of select="$record_el/Info/V2DStreamConfig/EnableSRDTranscode"/>
        </xsl:attribute>
        <xsl:attribute name="SRDTranscodeBandwidth">
            <xsl:value-of select="$record_el/Info/V2DStreamConfig/SRDTranscodeBandwidth"/>
        </xsl:attribute>
        <xsl:attribute name="AVOption">
            <xsl:value-of select="$record_el/Info/V2DStreamConfig/AVOption"/>
        </xsl:attribute>
        <xsl:attribute name="SampleRate">
            <xsl:value-of select="$record_el/Info/V2DEncoderAudioConfig/SampleRate"/>
        </xsl:attribute>
        <xsl:attribute name="SampleSize">
            <xsl:value-of select="$record_el/Info/V2DEncoderAudioConfig/SampleSize"/>
        </xsl:attribute>
        <xsl:attribute name="MonoStereo">
            <xsl:value-of select="$record_el/Info/V2DEncoderAudioConfig/MonoStereo"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/Info/V2DEncoderAudioConfig/AudioEncodeEnable='true'">
                <xsl:attribute name="AudioEncodeEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AudioEncodeEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="BurstRatio">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/BurstRatio"/>
        </xsl:attribute>
        <xsl:attribute name="BurstDuration">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/BurstDuration"/>
        </xsl:attribute>
        <xsl:attribute name="CompressionHigh">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Compression/High"/>
        </xsl:attribute>
        <xsl:attribute name="CompressionLow">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Compression/Low"/>
        </xsl:attribute>
        <xsl:attribute name="Bandwidth">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Bandwidth"/>
        </xsl:attribute>
        <xsl:attribute name="RefreshMin">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Refresh/Min"/>
        </xsl:attribute>
        <xsl:attribute name="RefreshMax">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Refresh/Max"/>
        </xsl:attribute>
        <xsl:attribute name="SliceMin">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Slice/Min"/>
        </xsl:attribute>
        <xsl:attribute name="SliceMax">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/Slice/Max"/>
        </xsl:attribute>
        <xsl:attribute name="BlockThreshold">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/BlockThreshold"/>
        </xsl:attribute>
        <xsl:attribute name="ColorSampling">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/ColorSampling"/>
        </xsl:attribute>
        <xsl:attribute name="FrameRateDivider">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterConfig/FrameRateDivider"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/Info/V2DEncoderConnectionParameterKbmConfig/AllowKMSwitch='true'">
                <xsl:attribute name="AllowKMSwitch">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowKMSwitch">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="KMIdleTime">
            <xsl:value-of select="$record_el/Info/V2DEncoderConnectionParameterKbmConfig/KMIdleTime"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/Info/V2DEncoderConnectionParameterKbmConfig/EnableKbm='true'">
                <xsl:attribute name="EnableKbm">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableKbm">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      <xsl:choose>
        <xsl:when test="$record_el/Info/V2DRecorderConfig/FFTracks='1-256'">
          <xsl:attribute name="FFTracks">1</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="FFTracks">0</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:template>
</xsl:stylesheet>