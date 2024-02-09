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
        <xsl:attribute name="V2DDecoderPortID">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/PortID"/>
        </xsl:attribute>
         <xsl:attribute name="V2DDecoderInstanceCount">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/MaxElementInstanceCount"/>
        </xsl:attribute>
         <xsl:attribute name="V2DDecoderStreamType">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/Info/StreamType"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DRxVideoParam">
            <xsl:value-of select="@DirtyBitblkV2DRxVideoParam"/>
        </xsl:attribute>
        <xsl:attribute name="RxPixelAdjust">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/PixelAdjust"/>
        </xsl:attribute>
        <xsl:attribute name="RxStereoSync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/StereoSync"/>
        </xsl:attribute>
        <xsl:attribute name="RxHSyncPolarity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/HSyncPolarity"/>
        </xsl:attribute>
        <xsl:attribute name="RxVSyncPolarity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/VSyncPolarity"/>
        </xsl:attribute>
        <xsl:attribute name="RxGenlockMode">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/GenlockMode"/>
        </xsl:attribute>
        <xsl:attribute name="RxMasterDelay">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig/MasterDelay"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DRxSerialParam">
            <xsl:value-of select="@DirtyBitblkV2DRxSerialParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/Enable='true'">
                <xsl:attribute name="RxSerialPort">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RxSerialPort">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="RxBaudRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/BaudRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxSerialDatabits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/DataBits"/>
        </xsl:attribute>
        <xsl:attribute name="RxSerialStopBits">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/StopBits"/>
        </xsl:attribute>
        <xsl:attribute name="RxSerialParity">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/Parity"/>
        </xsl:attribute>
        <xsl:attribute name="RxSerialFlowCntrl">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/FlowControl"/>
        </xsl:attribute>
        <xsl:attribute name="RxSerialMaxWait">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig/MaximumWaitTime"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkV2DRxSplashParam">
            <xsl:value-of select="@DirtyBitblkV2DRxSplashParam"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/EnableSplash='true'">
                <xsl:attribute name="RxSplashEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RxSplashEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="RxSplashScreen">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/SplashScreen"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/Stereo='true'">
                <xsl:attribute name="RxSplashEnableStereo">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RxSplashEnableStereo">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="RxSplashHres">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/Resolution/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVres">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/Resolution/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashRefreshRate">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashPixelClock">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHfrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/FrontPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVfrontPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/FrontPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHbackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/BackPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVbackPorch">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/BackPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashHsync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/Sync/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="RxSplashVsync">
            <xsl:value-of select="$record_el/DeviceConfig/AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig/Sync/Vertical"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>