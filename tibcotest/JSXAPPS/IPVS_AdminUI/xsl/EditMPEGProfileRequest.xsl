<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <MPEGTSMediaStreamProfile>
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
            <MPEGTSStreamConfig>
              <IsMulticast>
                <xsl:choose>
                  <xsl:when test="@MPEGIsMulticast='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose> 
              </IsMulticast> 
            </MPEGTSStreamConfig>
            <MPEGEncoderParameterConfig>
              <SDP-a-Template>
                <xsl:value-of select="@SDP-a-Template"/>
              </SDP-a-Template>
              <Encapsulation>
                <xsl:value-of select="@Encapsulation"/>
              </Encapsulation>
              <AudioCompression>
                <xsl:value-of select="@AudioCompression"/>
              </AudioCompression>
              <AudioBitrate>
                <xsl:value-of select="@AudioBitrate"/>
              </AudioBitrate>
              <VideoCompression>
                <xsl:value-of select="@VideoCompression"/>
              </VideoCompression>
              <VideoBitrate>
                <xsl:value-of select="@VideoBitrate"/>
              </VideoBitrate>
              <PictureSize>
                <xsl:value-of select="@PictureSize"/>
              </PictureSize>
              <VideoInputSource>
                <xsl:value-of select="@VideoInputSource"/>
              </VideoInputSource>
              <VideoFormat>
                <xsl:value-of select="@VideoFormat"/>
              </VideoFormat>
              <DeinterlaceMode>
                <xsl:value-of select="@DeinterlaceMode"/>
              </DeinterlaceMode>
              <Hue>
                <xsl:value-of select="@Hue"/>
              </Hue>
              <Saturation>
                <xsl:value-of select="@Saturation"/>
              </Saturation>
              <Brightness>
                <xsl:value-of select="@Brightness"/>
              </Brightness>
              <Contrast>
                <xsl:value-of select="@Contrast"/>
              </Contrast>
             </MPEGEncoderParameterConfig>
          </Info> 
        </MPEGTSMediaStreamProfile>
    </xsl:template>

 </xsl:stylesheet>
