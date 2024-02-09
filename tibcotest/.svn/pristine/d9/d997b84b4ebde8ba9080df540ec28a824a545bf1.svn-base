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
        <xsl:attribute name="DirtyBitblkPortInfo">
            <xsl:value-of select="@DirtyBitblkPortInfo"/>
        </xsl:attribute> 
        <xsl:attribute name="XP200PortTypeConfigurable">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/Info/Configurable"/>
        </xsl:attribute> 
        <xsl:attribute name="XP200PortTypeSource">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/Info/PortIOType"/>
        </xsl:attribute> 
        <xsl:attribute name="XP200PortTypeDestination">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/Info/PortIOType"/>
        </xsl:attribute> 
        <xsl:attribute name="XP200PortIDShow">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/PortID"/>
        </xsl:attribute>
         <xsl:attribute name="XP200StreamType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/Info/StreamType"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DTxVideoParam">
            <xsl:value-of select="@DirtyBitblkV2DTxVideoParam"/>
        </xsl:attribute>
        <xsl:attribute name="SamplingPhase">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/SamplingPhase"/>
        </xsl:attribute>
        <xsl:attribute name="CoarseNoise">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/NoiseElimination/Coarse"/>
        </xsl:attribute>
        <xsl:attribute name="FineNoise">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/NoiseElimination/Fine"/>
        </xsl:attribute>
        <xsl:attribute name="HorizontalPosition">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/Position/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="VerticalPosition">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/Position/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="LUTChangePump">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig/LUTPumpCurr"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DPortKeyboardMouse">
            <xsl:value-of select="@DirtyBitblkV2DPortKeyboardMouse"/>
        </xsl:attribute>
        <xsl:attribute name="KeyboardType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortKbmConfig/KbType"/>
        </xsl:attribute>
        <xsl:attribute name="MouseType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortKbmConfig/MouseType"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/EnableRFB='true'">
                <xsl:attribute name="EnableRemoteMouse">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableRemoteMouse">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="HostName">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/RFBHostName"/>
        </xsl:attribute>
        <xsl:attribute name="Password">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/Password"/>
        </xsl:attribute>
        <xsl:attribute name="Port">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/Port"/>
        </xsl:attribute>
        <xsl:attribute name="XOffset">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/XOffset"/>
        </xsl:attribute>
        <xsl:attribute name="YOffset">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderPortRFBConfig/YOffset"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DIOParam">
            <xsl:value-of select="@DirtyBitblkV2DIOParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/Enable='true'">
                <xsl:attribute name="SerialPortEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="SerialPortEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="BaudRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/BaudRate"/>
        </xsl:attribute>
        <xsl:attribute name="DataBits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/DataBits"/>
        </xsl:attribute>
        <xsl:attribute name="StopBits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/StopBits"/>
        </xsl:attribute>
        <xsl:attribute name="Parity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/Parity"/>
        </xsl:attribute>
        <xsl:attribute name="FlowControl">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/FlowControl"/>
        </xsl:attribute>
        <xsl:attribute name="MaximumWaitTime">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig/MaximumWaitTime"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DTxSplashParam">
            <xsl:value-of select="@DirtyBitblkV2DTxSplashParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/EnableSplash='true'">
                <xsl:attribute name="SplashScreenEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="SplashScreenEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="SplashScreen">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/SplashScreen"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/Stereo='true'">
                <xsl:attribute name="EnableStereo">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="EnableStereo">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="SplashHorizontalResolution">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/Resolution/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalResolution">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/Resolution/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashRefreshRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="SplashPixelClickRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorixontalFrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/FrontPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalFrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/FrontPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorizontalBackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/BackPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalBackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/BackPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="SplashHorizontalSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/Sync/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="SplashVerticalSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig/Sync/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DRxSplashParam">
            <xsl:value-of select="@DirtyBitblkV2DRxSplashParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/EnableSplash='true'">
                <xsl:attribute name="RxSplashEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RxSplashEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="RxSplashScreen">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/SplashScreen"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/Stereo='true'">
                <xsl:attribute name="RxSplashEnableStereo">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RxSplashEnableStereo">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="RxSplashHres">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/Resolution/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVres">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/Resolution/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashRefreshRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashPixelClock">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHfrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/FrontPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVfrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/FrontPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHbackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/BackPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVbackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/BackPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHsync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/Sync/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVsync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig/Sync/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DRxVideoParam">
            <xsl:value-of select="@DirtyBitblkV2DRxVideoParam"/>
        </xsl:attribute>
        <xsl:attribute name="RxPixelAdjust">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/PixelAdjust"/>
        </xsl:attribute>
        <xsl:attribute name="RxStereoSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/StereoSync"/>
        </xsl:attribute>
        <xsl:attribute name="RxHSyncPolarity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/HSyncPolarity"/>
        </xsl:attribute>
        <xsl:attribute name="RxVSyncPolarity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/VSyncPolarity"/>
        </xsl:attribute>
        <xsl:attribute name="RxGenlockMode">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/GenlockMode"/>
        </xsl:attribute>
        <xsl:attribute name="RxMasterDelay">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig/MasterDelay"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>