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
        <xsl:attribute name="DirtyBitblkV2DAVConfig">
            <xsl:value-of select="@DirtyBitblkV2DAVConfig"/>
        </xsl:attribute> 
        <xsl:attribute name="V2DEncoderPortID">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/PortID"/>
        </xsl:attribute>
         <xsl:attribute name="V2DEncoderInstanceCount">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/MaxElementInstanceCount"/>
        </xsl:attribute>
         <xsl:attribute name="V2DEncoderStreamtype">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/Info/StreamType"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DTxVideoParam">
            <xsl:value-of select="@DirtyBitblkV2DTxVideoParam"/>
        </xsl:attribute>
        <xsl:attribute name="SamplingPhase">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/SamplingPhase"/>
        </xsl:attribute>
        <xsl:attribute name="CoarseNoise">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/NoiseElimination/Coarse"/>
        </xsl:attribute>
        <xsl:attribute name="FineNoise">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/NoiseElimination/Fine"/>
        </xsl:attribute>
        <xsl:attribute name="HorizontalPosition">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/Position/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="VerticalPosition">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/Position/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="LUTChangePump">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/LUTPumpCurr"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DPortKeyboardMouse">
            <xsl:value-of select="@DirtyBitblkV2DPortKeyboardMouse"/>
        </xsl:attribute>
        <xsl:attribute name="KeyboardType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortKbmConfig/KbType"/>
        </xsl:attribute>
        <xsl:attribute name="MouseType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortKbmConfig/MouseType"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/EnableRFB='true'">
                <xsl:attribute name="EnableRemoteMouse">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableRemoteMouse">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="HostName">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/RFBHostName"/>
        </xsl:attribute>
        <xsl:attribute name="Password">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/Password"/>
        </xsl:attribute>
        <xsl:attribute name="Port">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/Port"/>
        </xsl:attribute>
        <xsl:attribute name="XOffset">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/XOffset"/>
        </xsl:attribute>
        <xsl:attribute name="YOffset">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortRFBConfig/YOffset"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DIOParam">
            <xsl:value-of select="@DirtyBitblkV2DIOParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/Enable='true'">
                <xsl:attribute name="SerialPortEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="SerialPortEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="BaudRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/BaudRate"/>
        </xsl:attribute>
        <xsl:attribute name="DataBits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/DataBits"/>
        </xsl:attribute>
        <xsl:attribute name="StopBits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/StopBits"/>
        </xsl:attribute>
        <xsl:attribute name="Parity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/Parity"/>
        </xsl:attribute>
        <xsl:attribute name="FlowControl">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/FlowControl"/>
        </xsl:attribute>
        <xsl:attribute name="MaximumWaitTime">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/MaximumWaitTime"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DTxSplashParam">
            <xsl:value-of select="@DirtyBitblkV2DTxSplashParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/EnableSplash='true'">
                <xsl:attribute name="SplashScreenEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="SplashScreenEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="SplashScreen">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/SplashScreen"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/Stereo='true'">
                <xsl:attribute name="EnableStereo">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableStereo">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="SplashHorizontalResolution">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/Resolution/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalResolution">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/Resolution/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashRefreshRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="SplashPixelClickRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorixontalFrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/FrontPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalFrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/FrontPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorizontalBackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/BackPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalBackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/BackPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorizontalSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/Sync/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/Sync/Vertical"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>