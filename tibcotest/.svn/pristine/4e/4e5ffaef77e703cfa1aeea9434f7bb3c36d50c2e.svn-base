<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <ProfileXML>
              <V2DStreamConfig>
                  <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkVideoConfigCnt"/>
                  </xsl:attribute>
                  <IsMulticast>true</IsMulticast> 
                  <EnableSRDTranscode></EnableSRDTranscode>
                  <SRDTranscodeBandwidth></SRDTranscodeBandwidth>
                  <AVOption>
                    <xsl:value-of select="@AVOption"/>
                  </AVOption>
              </V2DStreamConfig>
              <V2DEncoderAudioConfig>
                <SampleRate></SampleRate>
                <SampleSize></SampleSize>
                <MonoStereo></MonoStereo>
                <AudioEncodeEnable></AudioEncodeEnable>
              </V2DEncoderAudioConfig>
              <V2DEncoderConnectionParameterConfig>
                <xsl:attribute name="Dirty">
                    <xsl:value-of select="@DirtyBitblkVideoConfigCnt"/>
                </xsl:attribute>
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
                <IFrameInterval></IFrameInterval>
              </V2DEncoderConnectionParameterConfig>

              <V2DEncoderConnectionParameterKbmConfig>
                <xsl:attribute name="Dirty">
                    <xsl:value-of select="@DirtyBitblkAssetAutoStart"/>
                </xsl:attribute>
                <AllowKMSwitch>
                  <xsl:choose>
                      <xsl:when test="@AllowKMSwitch='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                  </xsl:choose>
                </AllowKMSwitch>
                <KMIdleTime></KMIdleTime>
                <EnableKbm>
                    <xsl:choose>
                      <xsl:when test="@EnableKbm='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                  </xsl:choose>
                </EnableKbm>
              </V2DEncoderConnectionParameterKbmConfig>
              <V2DRecorderConfig>
                <xsl:attribute name="Dirty">
                    <xsl:value-of select="@DirtyBitblkAssetAutoStart"/>
                </xsl:attribute>
                <FFTracks>
                  <xsl:choose>
                      <xsl:when test="@FFTracks='1'">true</xsl:when>
                      <xsl:otherwise>false</xsl:otherwise>
                  </xsl:choose>
                </FFTracks>

                <ThumbnailConfig>
                   <Enable></Enable>
                </ThumbnailConfig>
             </V2DRecorderConfig>
           </ProfileXML>
    </xsl:template>

 </xsl:stylesheet>
