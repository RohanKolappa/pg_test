<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <RTPMediaStreamProfile>
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
            <SDP-Template-Src>
                <xsl:value-of select="@SDP-Template-Src"/>
            </SDP-Template-Src>
            <RTPStreamConfig>
              <IsMulticast>
                <xsl:choose>
                  <xsl:when test="@RTPIsMulticast='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose> 
              </IsMulticast> 
              <SDP-m-Template>
                <xsl:value-of select="@SDP-m-Template"/>
              </SDP-m-Template>
              <AVOption>
                <xsl:value-of select="@RTPAVOption"/>
              </AVOption>
            </RTPStreamConfig>
            <MPEGEncoderParameterConfig>
              <SDP-a-Template>
                <xsl:value-of select="@RTPSDP-a-Template"/>
              </SDP-a-Template>
              <Encapsulation>
                <xsl:value-of select="@RTPEncapsulation"/>
              </Encapsulation>
              <AudioCompression>
                <xsl:value-of select="@RTPAudioCompression"/>
              </AudioCompression>
              <AudioBitrate>
                <xsl:value-of select="@RTPAudioBitrate"/>
              </AudioBitrate>
              <VideoCompression>
                <xsl:value-of select="@RTPVideoCompression"/>
              </VideoCompression>
              <VideoBitrate>
                <xsl:value-of select="@RTPVideoBitrate"/>
              </VideoBitrate>
              <PictureSize>
                <xsl:value-of select="@RTPPictureSize"/>
              </PictureSize>
              <VideoInputSource>
                <xsl:value-of select="@RTPVideoInputSource"/>
              </VideoInputSource>
              <VideoFormat>
                <xsl:value-of select="@RTPVideoFormat"/>
              </VideoFormat>
              <DeinterlaceMode>
                <xsl:value-of select="@RTPDeinterlaceMode"/>
              </DeinterlaceMode>
              <Hue>
                <xsl:value-of select="@RTPHue"/>
              </Hue>
              <Saturation>
                <xsl:value-of select="@RTPSaturation"/>
              </Saturation>
              <Brightness>
                <xsl:value-of select="@RTPBrightness"/>
              </Brightness>
              <Contrast>
                <xsl:value-of select="@RTPContrast"/>
              </Contrast>
             </MPEGEncoderParameterConfig>
             <RTPRecoderConfig>
                 <ThumbnailConfig>
                    <Enable>true</Enable>
                 </ThumbnailConfig>
               <HLSConfig>
                 <Enable>
                   <xsl:choose>
                     <xsl:when test="@EnableHLS='1'">true</xsl:when>
                     <xsl:otherwise>false</xsl:otherwise>
                   </xsl:choose>
                 </Enable>
               </HLSConfig>

             </RTPRecoderConfig>
          </Info> 

        </RTPMediaStreamProfile>
    </xsl:template>

 </xsl:stylesheet>
