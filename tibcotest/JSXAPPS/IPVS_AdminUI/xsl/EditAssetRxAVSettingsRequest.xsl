<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <DeviceConfig>
              <AVCodecElementsConfig>
                <V2DDecoderConfig>
                  <xsl:attribute name="Dirty">
                     <xsl:value-of select="@DirtyBitblkV2DAVConfig"/>
                  </xsl:attribute>
                  <PortID>
                      <xsl:value-of select="@V2DDecoderPortID"/>
                  </PortID> 
                  <MaxElementInstanceCount>
                      <xsl:value-of select="@V2DDecoderInstanceCount"/>
                  </MaxElementInstanceCount>
                  <Info>
                    <StreamType>
                      <xsl:value-of select="@V2DDecoderStreamType"/>
                    </StreamType>
                  </Info>
                  <V2DDecoderVideoConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DRxVideoParam"/>
                    </xsl:attribute>
                    <PixelAdjust>
                       <xsl:value-of select="@RxPixelAdjust"/>
                    </PixelAdjust> 
                    <StereoSync>
                       <xsl:value-of select="@RxStereoSync"/>
                    </StereoSync>
                    <HSyncPolarity>
                       <xsl:value-of select="@RxHSyncPolarity"/>
                    </HSyncPolarity>
                    <VSyncPolarity>
                       <xsl:value-of select="@RxVSyncPolarity"/>
                    </VSyncPolarity>
                    <GenlockMode>
                       <xsl:value-of select="@RxGenlockMode"/>
                    </GenlockMode>
                    <MasterDelay>
                       <xsl:value-of select="@RxMasterDelay"/>
                    </MasterDelay>
                  </V2DDecoderVideoConfig>
                  <V2DDecoderSerialConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DRxSerialParam"/>
                    </xsl:attribute>
                    <Enable>
                        <xsl:choose>
                          <xsl:when test="@RxSerialPort='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </Enable> 
                    <BaudRate>
                       <xsl:value-of select="@RxBaudRate"/>
                    </BaudRate> 
                    <DataBits>
                       <xsl:value-of select="@RxSerialDatabits"/>
                    </DataBits> 
                    <StopBits>
                       <xsl:value-of select="@RxSerialStopBits"/>
                    </StopBits> 
                    <Parity>
                       <xsl:value-of select="@RxSerialParity"/>
                    </Parity> 
                    <FlowControl>
                       <xsl:value-of select="@RxSerialFlowCntrl"/>
                    </FlowControl> 
                    <MaximumWaitTime>
                       <xsl:value-of select="@RxSerialMaxWait"/>
                    </MaximumWaitTime> 
                  </V2DDecoderSerialConfig>
                  <V2DDecoderSplashParametersConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DRxSplashParam"/>
                    </xsl:attribute>
                    <EnableSplash>
                        <xsl:choose>
                          <xsl:when test="@RxSplashEnable='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </EnableSplash> 
                    <SplashScreen>
                       <xsl:value-of select="@RxSplashScreen"/>
                    </SplashScreen> 
                    <Stereo>
                        <xsl:choose>
                          <xsl:when test="@RxSplashEnableStereo='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </Stereo> 
                    <Resolution>
                        <Horizontal>
                           <xsl:value-of select="@RxSplashHres"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@RxSplashVres"/>
                        </Vertical>
                    </Resolution> 
                    <RefreshRate>
                       <xsl:value-of select="@RxSplashRefreshRate"/>
                    </RefreshRate> 
                    <PixelClockRate>
                       <xsl:value-of select="@RxSplashPixelClock"/>
                    </PixelClockRate> 
                    <FrontPorch>
                        <Horizontal>
                           <xsl:value-of select="@RxSplashHfrontPorch"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@RxSplashVfrontPorch"/>
                        </Vertical>
                    </FrontPorch> 
                    <BackPorch>
                        <Horizontal>
                           <xsl:value-of select="@RxSplashHbackPorch"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@RxSplashVbackPorch"/>
                        </Vertical>
                    </BackPorch> 
                    <Sync>
                        <Horizontal>
                           <xsl:value-of select="@RxSplashHsync"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@RxSplashVsync"/>
                        </Vertical>
                    </Sync> 
                  </V2DDecoderSplashParametersConfig>
                  <V2DDecoderVideoLookupTable>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DAVConfig"/>
                    </xsl:attribute>
                  </V2DDecoderVideoLookupTable> 
                </V2DDecoderConfig>
              </AVCodecElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
