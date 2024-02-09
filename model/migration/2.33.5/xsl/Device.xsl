<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="/*/@serviceVer">
		<xsl:attribute name="serviceVer">
		<xsl:text>2.33.6</xsl:text>
	</xsl:attribute>
	</xsl:template>
	<xsl:template mode="append"
		match="*[starts-with(name(),'DMS') or starts-with(name(),'MS') or starts-with(name(),'SG')]/*/ServiceElementsConfig">
		<DemosModeConfig Dirty="CLEAN">
			<Enable>false</Enable>
		</DemosModeConfig>
	</xsl:template>
	<xsl:template mode="append"
		match="//V2D-XPi-Device/*/SystemElementsConfig">
		<OperationModeConfig Dirty="CLEAN">
			<Standalone>false</Standalone>
		</OperationModeConfig>
	</xsl:template>
	<xsl:template mode="append" match="//V2D-XPi-Device/*/AVCodecElementsConfig/XP200PortConfig">
		<V2DEncoderConnectionConfig Dirty="CLEAN">
			<MaxBandwidth>10M</MaxBandwidth>
			<BurstRatio></BurstRatio>
			<BurstDuration></BurstDuration>
			<LowCompression></LowCompression>
			<HighCompression></HighCompression>
			<AVOption></AVOption>
			<FrameRateDivider></FrameRateDivider>
			<MinRefresh></MinRefresh>
			<MaxRefresh></MaxRefresh>
			<MinSlice></MinSlice>
			<MaxSlice></MaxSlice>
			<BlockThreshold></BlockThreshold>
			<ColorSampling></ColorSampling>
			<AllowKBMSwitch></AllowKBMSwitch>
			<KBMIdleLimit></KBMIdleLimit>
		</V2DEncoderConnectionConfig>


		<V2DDecoderKeyboardLayoutConfig Dirty="CLEAN">
			<KeyboardLayout>US</KeyboardLayout>
		</V2DDecoderKeyboardLayoutConfig>

		<V2DEncoderKeyboardLayoutConfig Dirty="CLEAN">
			<KeyboardLayout>US</KeyboardLayout>
		</V2DEncoderKeyboardLayoutConfig>

		<V2DDecoderConnectConfig Dirty="CLEAN">
			<IPAddress>US</IPAddress>
			<PortNumber></PortNumber>
			<Bandwidth></Bandwidth>
			<AVOption></AVOption>
			<Multicast>true</Multicast>
			<Bidirectional>true</Bidirectional>
		</V2DDecoderConnectConfig>

		<V2DEncoderStreamingProfileConfig
			Dirty="CLEAN">
			<Enable>true</Enable>
			<Profile></Profile>
			<Bandwidth></Bandwidth>
			<LowCompression></LowCompression>
			<HighCompression></HighCompression>
		</V2DEncoderStreamingProfileConfig>

		<V2DEncoderForceSplashScreenConfig
			Dirty="CLEAN">
			<ForceSplashScreen>false</ForceSplashScreen>
		</V2DEncoderForceSplashScreenConfig>

		<V2DConnectionEDIDParametersConfig
			Dirty="CLEAN">
			<Enable>true</Enable>
		</V2DConnectionEDIDParametersConfig>

		<V2DConnectionEDIDVideoInput Dirty="CLEAN">
			<VideoInput>Digital</VideoInput>
		</V2DConnectionEDIDVideoInput>
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
	<!-- Here's the boilerplate code -->
	<!-- By default, copy all nodes unchanged -->
	<xsl:template match="@* | node()">
		<xsl:apply-templates mode="before" select="." />
		<xsl:copy>
			<xsl:apply-templates select="@*" />
			<xsl:apply-templates mode="add-atts" select="." />
			<xsl:apply-templates mode="insert" select="." />
			<xsl:apply-templates />
			<xsl:apply-templates mode="append" select="." />
		</xsl:copy>
		<xsl:apply-templates mode="after" select="." />
	</xsl:template>

	<!-- By default, don't add anything -->
	<xsl:template mode="add-atts" match="*" />
	<xsl:template mode="insert" match="*" />
	<xsl:template mode="append" match="*" />
	<xsl:template mode="before" match="@* | node()" />
	<xsl:template mode="after" match="@* | node()" />


	<!-- copy device status only if serviceVer is of last migration -->
	<xsl:template match="//DeviceStatus[not(*)]">
		<xsl:variable name="rootname" select="name(/*)" />
		<xsl:variable name="objectxml_file">
			<xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')" />
		</xsl:variable>
		<xsl:variable name="objectXml_serviceVer"
			select="document($objectxml_file)/*/@serviceVer" />
		<xsl:choose>
			<xsl:when test='"2.33.6"=$objectXml_serviceVer'>
				<xsl:copy-of select="document($objectxml_file)//DeviceStatus" />
			</xsl:when>
			<xsl:otherwise>
				<DeviceStatus>
					skip copy
					target serviceVer
					<xsl:value-of select="$objectXml_serviceVer" />
					in objectXml
					<xsl:value-of select="$objectxml_file" />
				</DeviceStatus>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>

