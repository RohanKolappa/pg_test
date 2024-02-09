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
                <V2DEncoderConfig>
                  <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkV2DAVConfig"/>
                  </xsl:attribute>
                  <PortID>
                      <xsl:value-of select="@V2DEncoderPortID"/>
                  </PortID> 
                  <MaxElementInstanceCount>
                      <xsl:value-of select="@V2DEncoderInstanceCount"/>
                  </MaxElementInstanceCount>
                  <Info>
                    <StreamType>
                      <xsl:value-of select="@V2DEncoderStreamtype"/>
                    </StreamType>
                  </Info>
                  <V2DEncoderVideoConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DTxVideoParam"/>
                    </xsl:attribute>
                    <SamplingPhase>
                       <xsl:value-of select="@SamplingPhase"/>
                    </SamplingPhase> 
                    <NoiseElimination>
                        <Coarse>
                           <xsl:value-of select="@CoarseNoise"/>
                        </Coarse>
                        <Fine>
                           <xsl:value-of select="@FineNoise"/>
                        </Fine>
                    </NoiseElimination>
                    <Position>
                        <Horizontal>
                           <xsl:value-of select="@HorizontalPosition"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@VerticalPosition"/>
                        </Vertical>
                    </Position>
                    <LUTPumpCurr>
                       <xsl:value-of select="@LUTChangePump"/>
                    </LUTPumpCurr> 
                  </V2DEncoderVideoConfig>
                  <V2DEncoderPortKbmConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DPortKeyboardMouse"/>
                    </xsl:attribute>
                    <KbType>
                       <xsl:value-of select="@KeyboardType"/>
                    </KbType> 
                    <MouseType>
                       <xsl:value-of select="@MouseType"/>
                    </MouseType> 
                  </V2DEncoderPortKbmConfig>
                  <V2DEncoderPortRFBConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DPortKeyboardMouse"/>
                    </xsl:attribute>
                    <EnableRFB>
                        <xsl:choose>
                          <xsl:when test="@EnableRemoteMouse='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </EnableRFB>
                    <RFBHostName>
                       <xsl:value-of select="@HostName"/>
                    </RFBHostName> 
                    <Password>
                       <xsl:value-of select="@Password"/>
                    </Password> 
                    <Port>
                       <xsl:value-of select="@Port"/>
                    </Port> 
                    <XOffset>
                       <xsl:value-of select="@XOffset"/>
                    </XOffset> 
                    <YOffset>
                       <xsl:value-of select="@YOffset"/>
                    </YOffset> 
                  </V2DEncoderPortRFBConfig>
                  <V2DEncoderSerialConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DIOParam"/>
                    </xsl:attribute>
                    <Enable>
                        <xsl:choose>
                          <xsl:when test="@SerialPortEnable='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </Enable> 
                    <BaudRate>
                       <xsl:value-of select="@BaudRate"/>
                    </BaudRate> 
                    <DataBits>
                       <xsl:value-of select="@DataBits"/>
                    </DataBits> 
                    <StopBits>
                       <xsl:value-of select="@StopBits"/>
                    </StopBits> 
                    <Parity>
                       <xsl:value-of select="@Parity"/>
                    </Parity> 
                    <FlowControl>
                       <xsl:value-of select="@FlowControl"/>
                    </FlowControl> 
                    <MaximumWaitTime>
                       <xsl:value-of select="@MaximumWaitTime"/>
                    </MaximumWaitTime> 
                  </V2DEncoderSerialConfig>
                  <V2DEncoderSplashParametersConfig>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DTxSplashParam"/>
                    </xsl:attribute>
                    <EnableSplash>
                        <xsl:choose>
                          <xsl:when test="@SplashScreenEnable='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </EnableSplash> 
                    <SplashScreen>
                       <xsl:value-of select="@SplashScreen"/>
                    </SplashScreen> 
                    <Stereo>
                        <xsl:choose>
                          <xsl:when test="@EnableStereo='1'">true</xsl:when>
                          <xsl:otherwise>false</xsl:otherwise>
                        </xsl:choose>
                    </Stereo> 
                    <Resolution>
                        <Horizontal>
                           <xsl:value-of select="@SplashHorizontalResolution"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@SplashVerticalResolution"/>
                        </Vertical>
                    </Resolution> 
                    <RefreshRate>
                       <xsl:value-of select="@SplashRefreshRate"/>
                    </RefreshRate> 
                    <PixelClockRate>
                       <xsl:value-of select="@SplashPixelClickRate"/>
                    </PixelClockRate> 
                    <FrontPorch>
                        <Horizontal>
                           <xsl:value-of select="@SplashHorixontalFrontPorch"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@SplashVerticalFrontPorch"/>
                        </Vertical>
                    </FrontPorch> 
                    <BackPorch>
                        <Horizontal>
                           <xsl:value-of select="@SplashHorizontalBackPorch"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@SplashVerticalBackPorch"/>
                        </Vertical>
                    </BackPorch> 
                    <Sync>
                        <Horizontal>
                           <xsl:value-of select="@SplashHorizontalSync"/>
                        </Horizontal>
                        <Vertical>
                           <xsl:value-of select="@SplashVerticalSync"/>
                        </Vertical>
                    </Sync> 
                  </V2DEncoderSplashParametersConfig>
                  <V2DCodecVideoLookupTable>
                    <xsl:attribute name="Dirty">
                        <xsl:value-of select="@DirtyBitblkV2DVideoLookup"/>
                    </xsl:attribute>
                  </V2DCodecVideoLookupTable> 
                </V2DEncoderConfig>
              </AVCodecElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
