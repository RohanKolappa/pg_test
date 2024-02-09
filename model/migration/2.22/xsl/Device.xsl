<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.23</xsl:text>
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


    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.23"=$objectXml_serviceVer'>
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

	 <xsl:template match="//IPVSClient-Device//AVCodecElementsConfig">
		<AVCodecElementsConfig>
			<IPVSClientPortConfig Dirty="CLEAN">
				<PortID>1</PortID>
				<MaxElementInstanceCount>16</MaxElementInstanceCount>
				<Info>
					<StreamType>V2D</StreamType>
					<StreamType>RTP</StreamType>
					<StreamType>MPEGTS</StreamType>
				</Info>
				<IPVSClientDisplayConfig>
					<Display configurable="false"/>
					<Canvas>
						<Background enable="" zIndex="" content="">
							<ImageContent imageURL="" color=""/>
							<SessionContent enableConnPanel="" enableBookmarkPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""/>
						</Background>
					<Window id="Win1" enable="" content="">
							<ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId=""/>
							<ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""/>
						<Geometry zIndex="1" border="" vpos="0" hpos="0" vres="930" hres="1700" aspect="" transparency="100"/>
						<SubCanvas/>
						</Window>
					</Canvas>
				</IPVSClientDisplayConfig>
			</IPVSClientPortConfig>
		</AVCodecElementsConfig>
	</xsl:template>	 
</xsl:stylesheet>

