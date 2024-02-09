
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.31.6</xsl:attribute>
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


		<xsl:template
			match="//MediaStreamIOPortResource[Info/StreamTypeList/StreamType='V2D' and Info/Type='StreamSrc']/InstanceInfo/InstanceList/Instance[@mode!='AutoStart']/MediaStreamInfo/MediaStreamProfileInfo">
			<xsl:choose>
				<xsl:when test="V2DMediaStreamProfileInfo">
				
					<MediaStreamProfileInfo>
						<V2DMediaStreamProfileInfo>
							<V2DStreamConfig>
								<IsMulticast>true</IsMulticast>
								<EnableSRDTranscode>true</EnableSRDTranscode>
								<SRDTranscodeBandwidth></SRDTranscodeBandwidth>
								<AVOption>Both</AVOption>
							</V2DStreamConfig>
							<xsl:copy-of
								select="document('/home/onair/objectXml/V2DMediaStreamProfile.xml')//Info/V2DEncoderAudioConfig" />
							<xsl:copy-of
								select="document('/home/onair/objectXml/V2DMediaStreamProfile.xml')//Info/V2DEncoderConnectionParameterConfig" />
							<xsl:copy-of
								select="document('/home/onair/objectXml/V2DMediaStreamProfile.xml')//Info/V2DEncoderConnectionParameterKbmConfig" />
							<xsl:copy-of
								select="document('/home/onair/objectXml/V2DMediaStreamProfile.xml')//Info/V2DRecorderConfig" />
						</V2DMediaStreamProfileInfo>
					</MediaStreamProfileInfo>
					
				</xsl:when>
			</xsl:choose>

		</xsl:template>

		
		<xsl:template match="//Instance[@mode!='AutoStart']" mode='add-atts'>
			<xsl:if test="//MediaStreamIOPortResource[Info/StreamTypeList/StreamType='V2D' and Info/Type='StreamSrc']">
				<xsl:attribute name="mode">
					<xsl:text>Dynamic</xsl:text>
				</xsl:attribute>
			</xsl:if>
		</xsl:template>

</xsl:stylesheet>