<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="/">
	
		<MediaStreamDstPortResource serviceVer=""
			Ver="2.3" Rev="1" parentNID="default.mediastreamdstportresourcelist">
			<xsl:variable name="assetnid">
				<xsl:value-of select="/AssetResource/@NID" />
			</xsl:variable>
			<xsl:attribute name="NID">default.mediastreamdstportresourcelist.msdprl_<xsl:value-of
				select="substring-after($assetnid,'assetresourcelist.arl_')" /></xsl:attribute>
				
			<Contact>
				<OwnerUserJID></OwnerUserJID>
				<Presence></Presence>
			</Contact>

			<State>
				<StateFlag>Template</StateFlag>
				<StateMsg></StateMsg>
			</State>

			 <AssetResourceNID>
                <xsl:value-of select="$assetnid"/>
            </AssetResourceNID>

			<ProfileInfo>
				<ProfileNIDList>
					<ProfileNID></ProfileNID>
				</ProfileNIDList>
			</ProfileInfo>

			<PVRInfo enable="" trimLength="" mediaStreamFileResourceNID=""
				mediaStreamDirResourceNID="" sessionId=""></PVRInfo>

			<RelayInfo>
				<UseRelay>true</UseRelay>
				<UseRelayAndDirectConnect>true</UseRelayAndDirectConnect>

				<RelayPortList>
					<MediaStreamRelayPortResourceNID></MediaStreamRelayPortResourceNID>
				</RelayPortList>
			</RelayInfo>

			<ObserverInfo>
				<Observer xAuth="" monitor="">
					<UserNID></UserNID>
					<ResourceNID></ResourceNID>
				</Observer>
			</ObserverInfo>


			<Info>
				<xsl:copy-of select="/AssetResource/Info/Groups" />
				<Title>
					<xsl:value-of select="/AssetResource/Info/Title" />
				</Title>
				<Description></Description>

				<StreamTypeList>
					<StreamType>V2D</StreamType>
					<StreamType>RTP</StreamType>
					<StreamType>MPEG</StreamType>
					<StreamURLType></StreamURLType>
				</StreamTypeList>

				<DeviceElementID>1</DeviceElementID>

				<FeatureList>
					<Feature Name=""></Feature>
				</FeatureList>

				<Type>StreamDst</Type>
			</Info>

			<InstanceInfo>
				<MaxInstanceCount>16</MaxInstanceCount>

				<MediaStreamList>
					<MediaStream NID="" id="" role="" roomType="" roomNID=""
						roomID=""></MediaStream>
				</MediaStreamList>

				<InstanceList>
					<Instance id="" sessionId="" mode="Dynamic">
						<ResourceNID></ResourceNID>

						<MediaStreamInfo>
							<MediaStreamURL></MediaStreamURL>

							<MediaStreamProfileInfo>
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

								<RTP-RAWMediaStreamProfileInfo>
									<SDP-Template-Src></SDP-Template-Src>
									<RTPRAWStreamConfig>
										<SDP-m-Template></SDP-m-Template>
										<IsMulticast></IsMulticast>
										<AVOption></AVOption>
									</RTPRAWStreamConfig>
									<RTPRAWRecoderConfig>
										<ThumbnailConfig>
											<Enable></Enable>
										</ThumbnailConfig>
									</RTPRAWRecoderConfig>
								</RTP-RAWMediaStreamProfileInfo>

								<RTP-PCMMediaStreamProfileInfo>
									<SDP-Template-Src></SDP-Template-Src>
									<RTPPCMStreamConfig>
										<SDP-m-Template></SDP-m-Template>
										<IsMulticast></IsMulticast>
										<AVOption></AVOption>
									</RTPPCMStreamConfig>
									<RTPPCMRecoderConfig>
										<ThumbnailConfig>
											<Enable></Enable>
										</ThumbnailConfig>
									</RTPPCMRecoderConfig>
								</RTP-PCMMediaStreamProfileInfo>

								<RFBMediaStreamProfileInfo>
									<SDP-Template-Src></SDP-Template-Src>
									<RFBStreamConfig>
										<SDP-m-Template></SDP-m-Template>
										<IsMulticast></IsMulticast>
										<AVOption></AVOption>
									</RFBStreamConfig>
									<RFBRecoderConfig>
										<ThumbnailConfig>
											<Enable></Enable>
										</ThumbnailConfig>
									</RFBRecoderConfig>
								</RFBMediaStreamProfileInfo>

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

								<UDPMediaStreamProfileInfo>
									<UDPStreamConfig>
										<IsMulticast></IsMulticast>
									</UDPStreamConfig>
								</UDPMediaStreamProfileInfo>

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

								<DVIMediaStreamProfileInfo>
									<DVIStreamConfig>
										<IsDVII></IsDVII>
									</DVIStreamConfig>
								</DVIMediaStreamProfileInfo>
							</MediaStreamProfileInfo>
						</MediaStreamInfo>

						<DeviceStreamingServiceReceiveInstanceNID></DeviceStreamingServiceReceiveInstanceNID>
						<DeviceStreamingServiceDecoderInstanceNID></DeviceStreamingServiceDecoderInstanceNID>
					</Instance>
				</InstanceList>
			</InstanceInfo>
		</MediaStreamDstPortResource>
	</xsl:template>

</xsl:stylesheet>