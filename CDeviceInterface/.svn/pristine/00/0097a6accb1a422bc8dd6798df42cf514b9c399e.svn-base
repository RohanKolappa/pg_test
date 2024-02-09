<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

 <xsl:output method="text" />

 <!-- Get the Version attribute from the root node of input document -->
  <xsl:template match="/">
	<xsl:text>dstjid=</xsl:text>
    <xsl:value-of select="//destAgentJID"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>roomnid=</xsl:text>
    <xsl:value-of select="//roomNID"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>urlparams='</xsl:text>
	<xsl:text>ismulticast=</xsl:text>
    <xsl:value-of select="//IsMulticast"/>
	<xsl:text>?</xsl:text>
	<xsl:text>enablekbm=</xsl:text>
    <xsl:value-of select="//EnableKbm"/>
	<xsl:text>?</xsl:text>
	<xsl:text>bandwidth=</xsl:text>
    <xsl:value-of select="//Bandwidth"/>
	<xsl:text>?</xsl:text>
	<xsl:text>avoption=</xsl:text>
    <xsl:value-of select="//AVOption"/>
	<xsl:text>?authscheme=TOKEN?</xsl:text>
	<xsl:text>authdata=</xsl:text>
    <xsl:value-of select="//mediaDestNID"/>
	<xsl:text>'&#10;</xsl:text>
	<xsl:text>authdata=</xsl:text>
    <xsl:value-of select="//mediaDestNID"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>bandwidth=</xsl:text>
    <xsl:value-of select="//Bandwidth"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>mcastaddr=</xsl:text>
    <xsl:value-of select="//multicastIP"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>av_option=</xsl:text>
    <xsl:value-of select="//AVOption"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>burst_duration=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/BurstDuration"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>burst_ratio=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/BurstRatio"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>low_compression=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Compression/Low"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>high_compression=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Compression/High"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>framerate_divider=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/FrameRateDivider"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>min_refresh=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Refresh/Min"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>max_refresh=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Refresh/Max"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>min_slice=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Slice/Min"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>max_slice=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/Slice/Max"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>block_threshold=</xsl:text>
    <xsl:value-of select="//V2DEncoderConnectionParameterConfig/BlockThreshold"/>
	<xsl:text>&#10;</xsl:text>
	<xsl:text>color_sampling="</xsl:text>
    <xsl:value-of select="//ColorSampling"/>
	<xsl:text>"</xsl:text>
	<xsl:text>&#10;</xsl:text>

  </xsl:template>

</xsl:stylesheet>

