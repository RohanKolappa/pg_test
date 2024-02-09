<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
	<!--_common.xsl : include xsl template -->
    <xsl:import href="_common.xsl"/>
   
	<!--  define variables  -->
    <xsl:variable name="Ver">1.4</xsl:variable>
    <xsl:variable name="Rev">1</xsl:variable>
	 
 	<!-- call add attr on root -->
    <xsl:template mode="add-atts" match="/MediaStreamRelayPortResource">
        <!-- call add common attr - call-template addAttrs_rootEL to set serviceVer-->
		<xsl:call-template name="addAttrs_rootEL"/> 
    </xsl:template>





    <!-- DELETE ELEMENTS MediaStreamRelayPortResource ( were present in 'objectXml_1.0' but removed from 'objectXml_1.3') -->

    <xsl:template match="/MediaStreamRelayPortResource/Info/MediaTypeList"/>
    <xsl:template match="/MediaStreamRelayPortResource/InstanceInfo/InstanceList/ResourceInstance/StreamNID"/>
    <xsl:template match="/MediaStreamRelayPortResource/Contact/Reachable"/>
    <xsl:template match="/MediaStreamRelayPortResource/InstanceInfo/InstanceList/ResourceInstance/RoomNID"/>


    <!-- Add ELEMENTS MediaStreamRelayPortResource ( were absent in 'objectXml_1.0' but added to  'objectXml_1.3') -->

    <!--
        /MediaStreamRelayPortResource/ProfileInfo /MediaStreamRelayPortResource/Info/StreamTypeList/StreamURLType /MediaStreamRelayPortResource/Contact/Presence
        /MediaStreamRelayPortResource/InstanceInfo/InstanceList/ResourceInstance/UpstreamMediaStreamInfo
        /MediaStreamRelayPortResource/InstanceInfo/InstanceList/ResourceInstance/DownstreamMediaStreamInfo
    -->

    <xsl:template mode="append" match="/MediaStreamRelayPortResource">

        <xsl:choose>
            <xsl:when test="ProfileInfo">
            </xsl:when>
            <xsl:otherwise>
                <ProfileInfo>
                    <ProfileNIDList>
                        <ProfileNID/>
                    </ProfileNIDList>
                </ProfileInfo>
            </xsl:otherwise>
        </xsl:choose>

        <!--  add State if not there -->
        <xsl:choose>
            <xsl:when test="State">
            </xsl:when>
            <xsl:otherwise>
                <State>
                    <StateFlag>Offline</StateFlag>
                    <StateMsg></StateMsg>
                </State>
            </xsl:otherwise>
        </xsl:choose>

        <!--  add Contact if not there -->
        <xsl:choose>
            <xsl:when test="Contact">
            </xsl:when>
            <xsl:otherwise>
                <Contact>
                    <OwnerUserJID></OwnerUserJID>
                    <Presence></Presence>
                </Contact>
            </xsl:otherwise>
        </xsl:choose>

        <!--  bug : the  //XAuthInfo was optional in 1.0 but needed in 1.3-->
        <xsl:choose>
            <xsl:when test="XAuthInfo">
            </xsl:when>
            <xsl:otherwise>
                <XAuthInfo>
                    <ForceXAuth>true</ForceXAuth>

                    <XAuthOperatorList>
                        <XAuthUserJID></XAuthUserJID>
                        <XAuthResourceNID></XAuthResourceNID>
                    </XAuthOperatorList>
                </XAuthInfo>
            </xsl:otherwise>
        </xsl:choose>

        <!-- add optional //PVRInfo and //RelayInfo  -->
        <xsl:choose>
            <xsl:when test="PVRInfo">
            </xsl:when>
            <xsl:otherwise>

                <PVRInfo>
                    <PVRMediaStoreResourceFileNID></PVRMediaStoreResourceFileNID>
                    <PVROnRelay>true</PVROnRelay>
                </PVRInfo>

            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="RelayInfo">
            </xsl:when>
            <xsl:otherwise>
                <RelayInfo>
                    <UseRelay>true</UseRelay>
                    <UseRelayAndDirectConnect>true</UseRelayAndDirectConnect>

                    <RelayPortList>
                        <MediaStreamRelayPortResourceNID></MediaStreamRelayPortResourceNID>
                    </RelayPortList>
                </RelayInfo>

            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>

    <xsl:template mode="append" match="/MediaStreamRelayPortResource/Info/StreamTypeList">

        <xsl:choose>
            <xsl:when test="StreamURLType">
            </xsl:when>
            <xsl:otherwise>
                <StreamURLType/>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>

    <xsl:template mode="append" match="/MediaStreamRelayPortResource/Contact">
       
        <xsl:choose>
            <xsl:when test="Presence">
            </xsl:when>
            <xsl:otherwise>
                <Presence/>
            </xsl:otherwise>
        </xsl:choose>
        
        <xsl:choose>
            <xsl:when test="OwnerUserJID">
            </xsl:when>
            <xsl:otherwise>
                <OwnerUserJID/>
            </xsl:otherwise>
        </xsl:choose>
    
    </xsl:template>
    

    <xsl:template mode="append" match="/MediaStreamRelayPortResource/State">
        
        <xsl:choose>
            <xsl:when test="StateFlag">
            </xsl:when>
            <xsl:otherwise>
                <StateFlag>Offline</StateFlag>
            </xsl:otherwise>
        </xsl:choose>
        
        <xsl:choose>
            <xsl:when test="StateMsg">
            </xsl:when>
            <xsl:otherwise>
                <StateMsg/>
            </xsl:otherwise>
        </xsl:choose>        

    </xsl:template>
        

    <xsl:template mode="append" match="/MediaStreamRelayPortResource/InstanceInfo/InstanceList/ResourceInstance">

        <xsl:choose>
            <xsl:when test="UpstreamMediaStreamInfo">
            </xsl:when>
            <xsl:otherwise>

                <UpstreamMediaStreamInfo>
                    <MediaStreamURL/>

                    <MediaStreamProfileInfo>
                        <MPEGTSMediaStreamProfileInfo>
                            <MPEGTSStreamConfig>
                                <IsMulticast/>
                            </MPEGTSStreamConfig>

                            <MPEGEncoderParameterConfig>
                                <SDP-a-Template/>
                                <Encapsulation/>
                                <AudioCompression/>
                                <AudioBitrate/>
                                <VideoCompression/>
                                <VideoBitrate/>
                                <PictureSize/>
                                <VideoInputSource/>
                                <VideoFormat/>
                                <DeinterlaceMode/>
                                <Hue/>
                                <Saturation/>
                                <Brightness/>
                                <Contrast/>
                            </MPEGEncoderParameterConfig>
                        </MPEGTSMediaStreamProfileInfo>

                        <RTPMediaStreamProfileInfo>
                            <SDP-Template-Src/>

                            <RTPStreamConfig>
                                <SDP-m-Template/>
                                <IsMulticast/>
                                <AVOption/>
                            </RTPStreamConfig>

                            <MPEGEncoderParameterConfig>
                                <SDP-a-Template/>
                                <Encapsulation/>
                                <AudioCompression/>
                                <AudioBitrate/>
                                <VideoCompression/>
                                <VideoBitrate/>
                                <PictureSize/>
                                <VideoInputSource/>
                                <VideoFormat/>
                                <DeinterlaceMode/>
                                <Hue/>
                                <Saturation/>
                                <Brightness/>
                                <Contrast/>
                            </MPEGEncoderParameterConfig>
                        </RTPMediaStreamProfileInfo>

                        <UDPMediaStreamProfileInfo>
                            <UDPStreamConfig>
                                <IsMulticast/>
                            </UDPStreamConfig>
                        </UDPMediaStreamProfileInfo>

                        <V2DMediaStreamProfileInfo>
                            <V2DStreamConfig>
                                <IsMulticast/>
                                <EnableSRDTranscode/>
                                <SRDTranscodeBandwidth/>
                                <AVOption/>
                            </V2DStreamConfig>

                            <V2DEncoderAudioConfig>
                                <SampleRate/>
                                <SampleSize/>
                                <MonoStereo/>
                                <AudioEncodeEnable/>
                            </V2DEncoderAudioConfig>

                            <V2DEncoderConnectionParameterConfig>
                                <BurstRatio/>
                                <BurstDuration/>

                                <Compression>
                                    <High/>
                                    <Low/>
                                </Compression>

                                <Bandwidth/>

                                <Refresh>
                                    <Min/>
                                    <Max/>
                                </Refresh>

                                <Slice>
                                    <Min/>
                                    <Max/>
                                </Slice>

                                <BlockThreshold/>
                                <Enable444/>
                                <FrameRateDivider/>
                            </V2DEncoderConnectionParameterConfig>

                            <V2DEncoderConnectionParameterKbmConfig>
                                <AllowKMSwitch/>
                                <KMIdleTime/>
                                <EnableKbm/>
                            </V2DEncoderConnectionParameterKbmConfig>
                        </V2DMediaStreamProfileInfo>
                    </MediaStreamProfileInfo>
                </UpstreamMediaStreamInfo>

            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="DownstreamMediaStreamInfo">
            </xsl:when>
            <xsl:otherwise>

                <DownstreamMediaStreamInfo>
                    <MediaStreamURL/>

                    <MediaStreamProfileInfo>
                        <MPEGTSMediaStreamProfileInfo>
                            <MPEGTSStreamConfig>
                                <IsMulticast/>
                            </MPEGTSStreamConfig>

                            <MPEGEncoderParameterConfig>
                                <SDP-a-Template/>
                                <Encapsulation/>
                                <AudioCompression/>
                                <AudioBitrate/>
                                <VideoCompression/>
                                <VideoBitrate/>
                                <PictureSize/>
                                <VideoInputSource/>
                                <VideoFormat/>
                                <DeinterlaceMode/>
                                <Hue/>
                                <Saturation/>
                                <Brightness/>
                                <Contrast/>
                            </MPEGEncoderParameterConfig>
                        </MPEGTSMediaStreamProfileInfo>

                        <RTPMediaStreamProfileInfo>
                            <SDP-Template-Src/>

                            <RTPStreamConfig>
                                <SDP-m-Template/>
                                <IsMulticast/>
                                <AVOption/>
                            </RTPStreamConfig>

                            <MPEGEncoderParameterConfig>
                                <SDP-a-Template/>
                                <Encapsulation/>
                                <AudioCompression/>
                                <AudioBitrate/>
                                <VideoCompression/>
                                <VideoBitrate/>
                                <PictureSize/>
                                <VideoInputSource/>
                                <VideoFormat/>
                                <DeinterlaceMode/>
                                <Hue/>
                                <Saturation/>
                                <Brightness/>
                                <Contrast/>
                            </MPEGEncoderParameterConfig>
                        </RTPMediaStreamProfileInfo>

                        <UDPMediaStreamProfileInfo>
                            <UDPStreamConfig>
                                <IsMulticast/>
                            </UDPStreamConfig>
                        </UDPMediaStreamProfileInfo>

                        <V2DMediaStreamProfileInfo>
                            <V2DStreamConfig>
                                <IsMulticast/>
                                <EnableSRDTranscode/>
                                <SRDTranscodeBandwidth/>
                                <AVOption/>
                            </V2DStreamConfig>

                            <V2DEncoderAudioConfig>
                                <SampleRate/>
                                <SampleSize/>
                                <MonoStereo/>
                                <AudioEncodeEnable/>
                            </V2DEncoderAudioConfig>

                            <V2DEncoderConnectionParameterConfig>
                                <BurstRatio/>
                                <BurstDuration/>

                                <Compression>
                                    <High/>
                                    <Low/>
                                </Compression>

                                <Bandwidth/>

                                <Refresh>
                                    <Min/>
                                    <Max/>
                                </Refresh>

                                <Slice>
                                    <Min/>
                                    <Max/>
                                </Slice>

                                <BlockThreshold/>
                                <Enable444/>
                                <FrameRateDivider/>
                            </V2DEncoderConnectionParameterConfig>

                            <V2DEncoderConnectionParameterKbmConfig>
                                <AllowKMSwitch/>
                                <KMIdleTime/>
                                <EnableKbm/>
                            </V2DEncoderConnectionParameterKbmConfig>
                        </V2DMediaStreamProfileInfo>
                    </MediaStreamProfileInfo>
                </DownstreamMediaStreamInfo>

            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>


    <!--  bug : the  //XAuthInfo was optional in 1.0 but needed in 1.3-->
    <xsl:template mode="append" match="//XAuthInfo">

        <xsl:choose>
            <xsl:when test="ForceXAuth">
            </xsl:when>
            <xsl:otherwise>
                <ForceXAuth>true</ForceXAuth>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="XAuthOperatorList">
            </xsl:when>
            <xsl:otherwise>
                <XAuthOperatorList>
                    <XAuthUserJID></XAuthUserJID>
                    <XAuthResourceNID></XAuthResourceNID>
                </XAuthOperatorList>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>

    <!-- to test  -->
    <xsl:template mode="append" match="//XAuthInfo/XAuthOperatorList">
        <xsl:choose>
            <xsl:when test="XAuthUserJID">
            </xsl:when>
            <xsl:otherwise>
                <XAuthUserJID></XAuthUserJID>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="XAuthResourceNID">
            </xsl:when>
            <xsl:otherwise>
                <XAuthResourceNID></XAuthResourceNID>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>


    <!-- add optional //PVRInfo and //RelayInfo  -->
    <xsl:template mode="append" match="//PVRInfo">

        <xsl:choose>
            <xsl:when test="PVRMediaStoreResourceFileNID">
            </xsl:when>
            <xsl:otherwise>
                <PVRMediaStoreResourceFileNID></PVRMediaStoreResourceFileNID>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="PVROnRelay">
            </xsl:when>
            <xsl:otherwise>
                <PVROnRelay>true</PVROnRelay>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>

    <xsl:template mode="append" match="//RelayInfo">

        <xsl:choose>
            <xsl:when test="UseRelay">
            </xsl:when>
            <xsl:otherwise>
                <UseRelay>true</UseRelay>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="UseRelayAndDirectConnect">
            </xsl:when>
            <xsl:otherwise>
                <UseRelayAndDirectConnect>true</UseRelayAndDirectConnect>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="RelayPortList">
            </xsl:when>
            <xsl:otherwise>
                <RelayPortList>
                    <MediaStreamRelayPortResourceNID></MediaStreamRelayPortResourceNID>
                </RelayPortList>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>



    <xsl:template mode="append" match="//Info">

        <xsl:choose>
            <xsl:when test="Groups">
            </xsl:when>
            <xsl:otherwise>
                <Groups>
                    <!--  <Tag Name="" Value=""></Tag> -->
                </Groups>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="Title">
            </xsl:when>
            <xsl:otherwise>
                <Title></Title>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="Description">
            </xsl:when>
            <xsl:otherwise>
                <Description></Description>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="StreamTypeList">
            </xsl:when>
            <xsl:otherwise>
                <StreamTypeList>
                    <StreamType></StreamType>
                    <StreamURLType></StreamURLType>
                </StreamTypeList>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="DeviceElementID">
            </xsl:when>
            <xsl:otherwise>
                <DeviceElementID></DeviceElementID>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="Type">
            </xsl:when>
            <xsl:otherwise>
                <Type>StreamRelay</Type>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:template>

    <xsl:template match="//DeviceElementID">
        <DeviceElementID>0</DeviceElementID>
    </xsl:template>
    
</xsl:stylesheet>