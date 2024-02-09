<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <V2DMediaStreamProfile>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@Rev"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@Ver"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@serviceVer"/>
          </xsl:attribute>
          <Groups>
          </Groups> 
          <StreamType>
              <xsl:value-of select="@StreamType"/>
          </StreamType> 
          <Priority>
              <xsl:value-of select="@Priority"/>
          </Priority>
          <Title>
              <xsl:value-of select="@Title"/>
          </Title>
          <Description>
              <xsl:value-of select="@Description"/>
          </Description>
          <StreamActionInfo>
              <AllowPause>
                <xsl:choose>
                  <xsl:when test="@AllowPause='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
               </AllowPause>
              <AllowSkipback>
                <xsl:choose>
                  <xsl:when test="@AllowSkipback='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
              </AllowSkipback>
              <AllowSkipfwd>
                <xsl:choose>
                  <xsl:when test="@AllowSkipfwd='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
              </AllowSkipfwd>
          </StreamActionInfo>
          <Info>
            <V2DStreamConfig>
              <IsMulticast>
                <xsl:choose>
                  <xsl:when test="@IsMulticast='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose> 
              </IsMulticast> 
              <EnableSRDTranscode>
                <xsl:value-of select="@EnableSRDTranscode"/>
              </EnableSRDTranscode>
              <SRDTranscodeBandwidth>
                <xsl:value-of select="@SRDTranscodeBandwidth"/>
              </SRDTranscodeBandwidth>
              <AVOption>
                <xsl:value-of select="@AVOption"/>
              </AVOption>
            </V2DStreamConfig>
            <V2DEncoderAudioConfig>
              <SampleRate>
                <xsl:value-of select="@SampleRate"/>
              </SampleRate>
              <SampleSize>
                <xsl:value-of select="@SampleSize"/>
              </SampleSize>
              <MonoStereo>
                <xsl:value-of select="@MonoStereo"/>
              </MonoStereo>
              <AudioEncodeEnable>
                <xsl:choose>
                  <xsl:when test="@AudioEncodeEnable='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose> 
              </AudioEncodeEnable>
            </V2DEncoderAudioConfig>
            <V2DEncoderConnectionParameterConfig>
              <BurstRatio>
                <xsl:value-of select="@BurstRatio"/>
              </BurstRatio>
              <BurstDuration>
                <xsl:value-of select="@BurstDuration"/>
              </BurstDuration>
              <Compression>
              <High>
                <xsl:value-of select="@CompressionHigh"/>
              </High>
              <Low>
                <xsl:value-of select="@CompressionLow"/>
              </Low>
              </Compression>
              <Bandwidth>
                <xsl:value-of select="@Bandwidth"/>
              </Bandwidth>
              <Refresh>
              <Min>
                <xsl:value-of select="@RefreshMin"/>
              </Min>
              <Max>
                <xsl:value-of select="@RefreshMax"/>
              </Max>
              </Refresh>
              <Slice>
              <Min>
                <xsl:value-of select="@SliceMin"/>
              </Min>
              <Max>
                <xsl:value-of select="@SliceMax"/>
              </Max>
              </Slice>
              <BlockThreshold>
                <xsl:value-of select="@BlockThreshold"/>
              </BlockThreshold>
              <ColorSampling>
                <xsl:value-of select="@ColorSampling"/>
              </ColorSampling>
              <FrameRateDivider>
                <xsl:value-of select="@FrameRateDivider"/>
              </FrameRateDivider>
             </V2DEncoderConnectionParameterConfig>
            <V2DEncoderConnectionParameterKbmConfig>
              <AllowKMSwitch>
                <xsl:choose>
                  <xsl:when test="@AllowKMSwitch='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>                
              </AllowKMSwitch> 
              <KMIdleTime>
                <xsl:value-of select="@KMIdleTime"/>
              </KMIdleTime> 
              <EnableKbm>
                <xsl:choose>
                  <xsl:when test="@EnableKbm='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>                
              </EnableKbm> 
            </V2DEncoderConnectionParameterKbmConfig>
            <V2DRecorderConfig>
              <FFTracks>
                <xsl:choose>
                  <xsl:when test="@FFTracks='1'">1-256</xsl:when>
                  <xsl:otherwise></xsl:otherwise>
                </xsl:choose>
              </FFTracks>
              <ThumbnailConfig>
                <Enable>true</Enable>
              </ThumbnailConfig>
            </V2DRecorderConfig>


          </Info> 

        </V2DMediaStreamProfile>
    </xsl:template>

 </xsl:stylesheet>
