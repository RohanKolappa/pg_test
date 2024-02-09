<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    <xsl:template match="/">

        <MediaStreamDstPortResource serviceVer="" Ver="1.5" Rev="1" parentNID="default.mediastreamdstportresourcelist">
            <xsl:variable name="assetnid">
                <xsl:value-of select="/AssetResource/@NID"/>
            </xsl:variable>
            <xsl:attribute name="NID">default.mediastreamdstportresourcelist.msdprl_<xsl:value-of select="substring-after($assetnid,'assetresourcelist.')"/></xsl:attribute>
            <Contact>
                <OwnerUserJID></OwnerUserJID>
                <Presence></Presence>
            </Contact>

            <State>
                <StateFlag></StateFlag>
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

            <PVRInfo>
                <PVRMediaStoreResourceFileNID></PVRMediaStoreResourceFileNID>
                <PVROnRelay>true</PVROnRelay>
            </PVRInfo>

            <RelayInfo>
                <UseRelay>true</UseRelay>
                <UseRelayAndDirectConnect>true</UseRelayAndDirectConnect>

                <RelayPortList>
                    <MediaStreamRelayPortResourceNID></MediaStreamRelayPortResourceNID>
                </RelayPortList>
            </RelayInfo>

            <XAuthInfo>
                <ForceXAuth>true</ForceXAuth>

                <XAuthOperatorList>
                    <XAuthUserJID></XAuthUserJID>
                    <XAuthResourceNID></XAuthResourceNID>
                </XAuthOperatorList>
            </XAuthInfo>

            <Info>
                <xsl:copy-of select="/AssetResource/Info/Groups"/>
                <Title>
                    <xsl:value-of select="/AssetResource/Info/Title"/>
                </Title>
                <Description></Description>

                <StreamTypeList>
                    <StreamType></StreamType>
                    <StreamURLType></StreamURLType>
                </StreamTypeList>

                <DeviceElementID></DeviceElementID>
                <Type></Type>
            </Info>

            <InstanceInfo>
                <MaxInstanceCount></MaxInstanceCount>

                <InstanceList>
                    <ResourceInstance>
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
                                        <Enable444></Enable444>
                                        <FrameRateDivider></FrameRateDivider>
                                    </V2DEncoderConnectionParameterConfig>

                                    <V2DEncoderConnectionParameterKbmConfig>
                                        <AllowKMSwitch></AllowKMSwitch>
                                        <KMIdleTime></KMIdleTime>
                                        <EnableKbm></EnableKbm>
                                    </V2DEncoderConnectionParameterKbmConfig>
                                </V2DMediaStreamProfileInfo>
                            </MediaStreamProfileInfo>
                        </MediaStreamInfo>

                        <DeviceStreamingServiceReceiveInstanceNID></DeviceStreamingServiceReceiveInstanceNID>
                        <DeviceStreamingServiceDecoderInstanceNID></DeviceStreamingServiceDecoderInstanceNID>
                    </ResourceInstance>
                </InstanceList>
            </InstanceInfo>
        </MediaStreamDstPortResource>

    </xsl:template>

</xsl:stylesheet>