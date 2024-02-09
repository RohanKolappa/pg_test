<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	
	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.31.13</xsl:attribute>
	</xsl:template>
	
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">2.4</xsl:attribute>
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

	<xsl:template mode="append" match="//RTPMediaStreamProfileInfo/RTPRecoderConfig">
		<xsl:choose>
			<xsl:when test="HLSConfig">
			</xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="../../../MediaStreamURL[../../../../..//StreamType='RTP'] != ''">
						
						<xsl:variable name="streamURL" select="//MediaStreamURL"/>
						<xsl:variable name="videoCodec">
							<xsl:choose>
								<xsl:when test="contains($streamURL, 'm=video')">
									<xsl:variable name="videoPort" select="normalize-space(substring-before(normalize-space(substring-after(substring-after($streamURL, 'm=video'), 'RTP/AVP')), ' '))"/>
									<xsl:value-of select="normalize-space(substring-before(substring-after($streamURL, concat('a=rtpmap:', $videoPort)), '/'))"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="other"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:variable>			
						<!-- <xsl:value-of select="$videoCodec"/> -->		
						<xsl:variable name="audioCodec">
							<xsl:choose>
								<xsl:when test="contains($streamURL, 'm=audio')">
									<xsl:variable name="audioPort" select="normalize-space(substring-before(normalize-space(substring-after(substring-after($streamURL, 'm=audio'), 'RTP/AVP')), ' '))"/>
									<xsl:value-of select="normalize-space(substring-before(substring-after($streamURL, concat('a=rtpmap:', $audioPort)), '/'))"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="other"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:variable>
						<!-- <xsl:value-of select="$audioCodec"/> -->
						<xsl:choose>
							<xsl:when test="(($videoCodec='H264') and ($audioCodec='' or $audioCodec='mpeg4-generic'))">
								<HLSConfig>
									<Enable>true</Enable>
								</HLSConfig>
							</xsl:when>
							<xsl:otherwise>
								<HLSConfig>
									<Enable>false</Enable>
								</HLSConfig>
							</xsl:otherwise>
						</xsl:choose>
						
					</xsl:when>
					
					<xsl:otherwise>
						<HLSConfig>
							<Enable>false</Enable>
						</HLSConfig>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
		 
	
  <xsl:template match="//RTPStreamConfig/IsMulticast">
		<IsMulticast><xsl:value-of select="string('false')"/></IsMulticast>
  </xsl:template>
  
  <xsl:template match="//UDPStreamConfig/IsMulticast">
		<IsMulticast><xsl:value-of select="string('false')"/></IsMulticast>
  </xsl:template>
  
  <xsl:template match="//MPEGTSStreamConfig/IsMulticast">
		<IsMulticast><xsl:value-of select="string('false')"/></IsMulticast>
  </xsl:template>
    
</xsl:stylesheet>