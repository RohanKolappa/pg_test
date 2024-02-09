<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <RTP-RAWMediaStreamProfile>
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
                <xsl:value-of select="@RAWSDP-Template-Src"/>
            </SDP-Template-Src>
            <RTPRAWStreamConfig>
              <IsMulticast>
                <xsl:choose>
                  <xsl:when test="@RTPRAWIsMulticast='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose> 
              </IsMulticast> 
              <SDP-m-Template>
                <xsl:value-of select="@RAWSDP-m-Template"/>
              </SDP-m-Template>
              <AVOption>
                <xsl:value-of select="@RTPRAWAVOption"/>
              </AVOption>
            </RTPRAWStreamConfig>
             <RTPRAWRecoderConfig>
                 <ThumbnailConfig>
                    <Enable>true</Enable>
                 </ThumbnailConfig>
             </RTPRAWRecoderConfig>
          </Info> 

        </RTP-RAWMediaStreamProfile>
    </xsl:template>

 </xsl:stylesheet>
