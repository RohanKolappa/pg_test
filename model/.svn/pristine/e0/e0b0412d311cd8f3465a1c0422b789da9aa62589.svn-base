<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    

    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.6</xsl:text>
	</xsl:attribute>
    </xsl:template>

    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
    <xsl:template match="@* | node()">
        <xsl:apply-templates mode="before" select="."/>
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
            <xsl:apply-templates mode="add-atts" select="."/>
            <xsl:apply-templates mode="insert" select="."/>
            <xsl:apply-templates/>
            <xsl:apply-templates mode="append" select="."/>
        </xsl:copy>
        <xsl:apply-templates mode="after" select="."/>
    </xsl:template>

    <!-- By default, don't add anything -->
    <xsl:template mode="add-atts" match="*"/>
    <xsl:template mode="insert" match="*"/>
    <xsl:template mode="append" match="*"/>
    <xsl:template mode="before" match="@* | node()"/>
    <xsl:template mode="after" match="@* | node()"/>

    <!-- Modify splash config to match a standard VESA rate -->
    <xsl:template match="//V2DEncoderSplashParametersConfig">
        <V2DEncoderSplashParametersConfig>
          <SplashScreen>Blue Screen</SplashScreen>
          <EnableSplash>true</EnableSplash>
          <Stereo>false</Stereo>
          <Resolution>
            <Horizontal>1280</Horizontal>
            <Vertical>1024</Vertical>
          </Resolution>
          <RefreshRate>60.02</RefreshRate>
          <PixelClockRate>108.00</PixelClockRate>
          <FrontPorch>
            <Horizontal>48</Horizontal>
            <Vertical>1</Vertical>
          </FrontPorch>
          <BackPorch>
            <Horizontal>248</Horizontal>
            <Vertical>38</Vertical>
          </BackPorch>
          <Sync>
            <Horizontal>112</Horizontal>
            <Vertical>3</Vertical>
          </Sync>
          <SplashBandwidth>1.00M</SplashBandwidth>
        </V2DEncoderSplashParametersConfig>
    </xsl:template>

    <xsl:template match="//V2DDecoderSplashParametersConfig">
        <V2DDecoderSplashParametersConfig>
          <SplashScreen>Blue Screen</SplashScreen>
          <EnableSplash>true</EnableSplash>
          <Stereo>false</Stereo>
          <Resolution>
            <Horizontal>1280</Horizontal>
            <Vertical>1024</Vertical>
          </Resolution>
          <RefreshRate>60.02</RefreshRate>
          <PixelClockRate>108.00</PixelClockRate>
          <FrontPorch>
            <Horizontal>48</Horizontal>
            <Vertical>1</Vertical>
          </FrontPorch>
          <BackPorch>
            <Horizontal>248</Horizontal>
            <Vertical>38</Vertical>
          </BackPorch>
          <Sync>
            <Horizontal>112</Horizontal>
            <Vertical>3</Vertical>
          </Sync>
        </V2DDecoderSplashParametersConfig>
    </xsl:template>

    <!-- Delete unsupported default entries from LUT table -->
    <xsl:template match="//V2DCodecVideoLookupEntryConfig[Name='VESA 640x480@60Hz']">
    </xsl:template>
    <xsl:template match="//V2DCodecVideoLookupEntryConfig[Name='ONYX2 1280x1024 60.0 Hz']">
    </xsl:template>
    <xsl:template match="//V2DDecoderVideoLookupEntryConfig[Name='VESA 640x480@60Hz']">
    </xsl:template>
    <xsl:template match="//V2DDecoderVideoLookupEntryConfig[Name='ONYX2 1280x1024 60.0 Hz']">
    </xsl:template>

    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.6"=$objectXml_serviceVer'>
                <xsl:copy-of select="document($objectxml_file)//DeviceStatus"/>
            </xsl:when>
            <xsl:otherwise>
                <DeviceStatus>
                    skip copy 
                    target serviceVer <xsl:value-of select="$objectXml_serviceVer"/>
                    in objectXml <xsl:value-of select="$objectxml_file"/>
                </DeviceStatus>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    
</xsl:stylesheet>

