<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.30.2</xsl:attribute>
	</xsl:template>
	
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">2.0</xsl:attribute>
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

	<xsl:template match="//Instance" mode='add-atts'>
		<xsl:attribute name="mode">
			<xsl:text>Dynamic</xsl:text>
		</xsl:attribute>
		<xsl:attribute name="sessionId">
			<xsl:text></xsl:text>
		</xsl:attribute>
	</xsl:template>
	<xsl:template mode="append" match="//MediaStreamSrcPortResource[Info/StreamTypeList/StreamType='V2D']/InstanceInfo/InstanceList/Instance/MediaStreamInfo/MediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="V2DMediaStreamProfileInfo">
			</xsl:when>
			<xsl:otherwise>
				<V2DMediaStreamProfileInfo>
			                     <V2DStreamConfig>
			                        <IsMulticast></IsMulticast>
			                        <EnableSRDTranscode></EnableSRDTranscode>
			                        <SRDTranscodeBandwidth></SRDTranscodeBandwidth>
			                        <AVOption></AVOption>
			                     </V2DStreamConfig>
			
			                     <V2DEncoderAudioConfig>
			                        <SampleRate></SampleRate>
			                        <SampleSize></SampleSize>
			                        <MonoStereo></MonoStereo>
			                        <AudioEncodeEnable></AudioEncodeEnable>
			                     </V2DEncoderAudioConfig>
			
			                     <V2DEncoderConnectionParameterConfig>
			                        <BurstRatio></BurstRatio>
			                        <BurstDuration></BurstDuration>
			
			                        <Compression>
			                           <High></High>
			                           <Low></Low>
			                        </Compression>
			
			                        <Bandwidth></Bandwidth>
			
			                        <Refresh>
			                           <Min></Min>
			                           <Max></Max>
			                        </Refresh>
			
			                        <Slice>
			                           <Min></Min>
			                           <Max></Max>
			                        </Slice>
			
			                        <BlockThreshold></BlockThreshold>
			                        <ColorSampling></ColorSampling>
			                        <FrameRateDivider></FrameRateDivider>
			                        <IFrameInterval></IFrameInterval>
			                     </V2DEncoderConnectionParameterConfig>
			
			                     <V2DEncoderConnectionParameterKbmConfig>
			                        <AllowKMSwitch></AllowKMSwitch>
			                        <KMIdleTime></KMIdleTime>
			                        <EnableKbm></EnableKbm>
			                     </V2DEncoderConnectionParameterKbmConfig>
			
			                     <V2DRecorderConfig>
			                        <FFTracks></FFTracks>
			
			                        <ThumbnailConfig>
			                           <Enable></Enable>
			                        </ThumbnailConfig>
			                     </V2DRecorderConfig>
			                  </V2DMediaStreamProfileInfo>
			</xsl:otherwise>							
		</xsl:choose>
	</xsl:template>	
	<xsl:template mode="append" match="//MediaStreamSrcPortResource[Info/StreamTypeList/StreamType='MPEGTS']/InstanceInfo/InstanceList/Instance/MediaStreamInfo/MediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="MPEGTSMediaStreamProfileInfo">
			</xsl:when>
			<xsl:otherwise>
				   <MPEGTSMediaStreamProfileInfo>
                     <MPEGTSStreamConfig>
                        <IsMulticast></IsMulticast>
                     </MPEGTSStreamConfig>

                     <MPEGEncoderParameterConfig>
                        <SDP-a-Template></SDP-a-Template>
                        <Encapsulation></Encapsulation>
                        <AudioCompression></AudioCompression>
                        <AudioBitrate></AudioBitrate>
                        <VideoCompression></VideoCompression>
                        <VideoBitrate></VideoBitrate>
                        <PictureSize></PictureSize>
                        <VideoInputSource></VideoInputSource>
                        <VideoFormat></VideoFormat>
                        <DeinterlaceMode></DeinterlaceMode>
                        <Hue></Hue>
                        <Saturation></Saturation>
                        <Brightness></Brightness>
                        <Contrast></Contrast>
                     </MPEGEncoderParameterConfig>
                  </MPEGTSMediaStreamProfileInfo>
			</xsl:otherwise>							
		</xsl:choose>
	</xsl:template>	
		<xsl:template mode="append" match="//MediaStreamSrcPortResource[Info/StreamTypeList/StreamType='RTP']/InstanceInfo/InstanceList/Instance/MediaStreamInfo/MediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="RTPMediaStreamProfileInfo">
			</xsl:when>
			<xsl:otherwise>
				   <RTPMediaStreamProfileInfo>
                     <SDP-Template-Src></SDP-Template-Src>

                     <RTPStreamConfig>
                        <SDP-m-Template></SDP-m-Template>
                        <IsMulticast></IsMulticast>
                        <AVOption></AVOption>
                     </RTPStreamConfig>

                     <MPEGEncoderParameterConfig>
                        <SDP-a-Template></SDP-a-Template>
                        <Encapsulation></Encapsulation>
                        <AudioCompression></AudioCompression>
                        <AudioBitrate></AudioBitrate>
                        <VideoCompression></VideoCompression>
                        <VideoBitrate></VideoBitrate>
                        <PictureSize></PictureSize>
                        <VideoInputSource></VideoInputSource>
                        <VideoFormat></VideoFormat>
                        <DeinterlaceMode></DeinterlaceMode>
                        <Hue></Hue>
                        <Saturation></Saturation>
                        <Brightness></Brightness>
                        <Contrast></Contrast>
                     </MPEGEncoderParameterConfig>

                     <RTPRecoderConfig>
                        <ThumbnailConfig>
                           <Enable></Enable>
                        </ThumbnailConfig>
                     </RTPRecoderConfig>
                  </RTPMediaStreamProfileInfo>
			</xsl:otherwise>							
		</xsl:choose>
	</xsl:template>	
		<xsl:template mode="append" match="//MediaStreamSrcPortResource[Info/StreamTypeList/StreamType='UDP']/InstanceInfo/InstanceList/Instance/MediaStreamInfo/MediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="UDPMediaStreamProfileInfo">
			</xsl:when>
			<xsl:otherwise>
				    <UDPMediaStreamProfileInfo>
                     <UDPStreamConfig>
                        <IsMulticast></IsMulticast>
                     </UDPStreamConfig>
                  </UDPMediaStreamProfileInfo>
			</xsl:otherwise>							
		</xsl:choose>
	</xsl:template>	
		<xsl:template mode="append" match="//MediaStreamSrcPortResource[Info/StreamTypeList/StreamType='DVI']/InstanceInfo/InstanceList/Instance/MediaStreamInfo/MediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="DVIMediaStreamProfileInfo">
			</xsl:when>
			<xsl:otherwise>
				    <DVIMediaStreamProfileInfo>
                     <DVIStreamConfig>
                        <IsDVII></IsDVII>
                     </DVIStreamConfig>
                  </DVIMediaStreamProfileInfo>
			</xsl:otherwise>							
		</xsl:choose>
	</xsl:template>	
</xsl:stylesheet>

 
 
