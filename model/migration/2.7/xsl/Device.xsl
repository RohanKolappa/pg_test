<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.8</xsl:text>
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
	<xsl:template match="//XP200VideoEncoderConfig">
    </xsl:template>
	<xsl:template mode="after" match="//V2DEncoderVideoConfig/LUTPumpCurr">
		<AdvancedVideoEncoderConfig>
			<OptimizeLatency>false</OptimizeLatency>
			<DVIMode>Auto</DVIMode>
			<IFrameInterval>0</IFrameInterval>
		</AdvancedVideoEncoderConfig>
    </xsl:template>
	<xsl:template mode="after" match="//V2DDecoderVideoConfig">
		<XP200DecoderPortKbmConfig Dirty="CLEAN"> 
			<KbType>PC</KbType> 
			<MouseType>IMPS2</MouseType> 
			<KBMMode>Remote</KBMMode>
		</XP200DecoderPortKbmConfig>
		<XP200InputVideoConfig Dirty="CLEAN">
			<Position>
				<Horizontal>0</Horizontal>
				<Vertical>0</Vertical>
			</Position>
			<SamplingPhase>0</SamplingPhase>
			<LUTPumpCurr>Auto</LUTPumpCurr>
			<DVIMode>Auto</DVIMode>
		</XP200InputVideoConfig>
    </xsl:template>
	<xsl:template mode="after" match="//V2DEncoderPortKbmConfig/MouseType">
		<KBMLoopback>true</KBMLoopback>
    </xsl:template>

	<xsl:template match="//XP200DisplayConfig/OverlayType">
		<OverlayType>Manual</OverlayType>
    </xsl:template>
	<xsl:template match="//XP200DisplayConfig/Aspect">
		<Aspect>Auto</Aspect>
    </xsl:template>

    <!-- By default, don't add anything -->
    <xsl:template mode="add-atts" match="*"/>
    <xsl:template mode="insert" match="*"/>
    <xsl:template mode="append" match="*"/>
    <xsl:template mode="before" match="@* | node()"/>
    <xsl:template mode="after" match="@* | node()"/>


    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.8"=$objectXml_serviceVer'>
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

