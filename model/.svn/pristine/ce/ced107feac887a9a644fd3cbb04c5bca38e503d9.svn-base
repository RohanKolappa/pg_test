<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:param name="devname">
    </xsl:param>
    <xsl:variable name="dms100" select="'DMS-100-Device'"/>
    <xsl:variable name="mgmnt">
        <xsl:value-of select="//XMPPServerEnable"/>
    </xsl:variable>

    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.4</xsl:text>
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


   <xsl:template mode="after" match="//V2DEncoderSplashParametersConfig/Sync">
       <SplashBandwidth>1.00M</SplashBandwidth>      
   </xsl:template>

   <xsl:template mode="after" match="//XP200PortConfig/V2DEncoderSplashParametersConfig">
       <V2DDecoderSplashParametersConfig>
          <SplashScreen>Blue Screen</SplashScreen>
          <EnableSplash>true</EnableSplash>
          <Stereo>false</Stereo>
          <Resolution>
            <Horizontal>1280</Horizontal>
            <Vertical>1024</Vertical>
          </Resolution>
          <RefreshRate>60.00</RefreshRate>
          <PixelClockRate>107.350</PixelClockRate>
          <FrontPorch>
            <Horizontal>40</Horizontal>
            <Vertical>3</Vertical>
          </FrontPorch>
          <BackPorch>
            <Horizontal>240</Horizontal>
            <Vertical>35</Vertical>
          </BackPorch>
          <Sync>
            <Horizontal>120</Horizontal>
            <Vertical>3</Vertical>
          </Sync>
        </V2DDecoderSplashParametersConfig>     
   </xsl:template>
   
    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.4"=$objectXml_serviceVer'>
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

