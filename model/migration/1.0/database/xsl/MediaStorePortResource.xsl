<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
	<!--_common.xsl : include xsl template -->
    <xsl:import href="_common.xsl"/>
   
	<!--  define variables  -->
    <xsl:variable name="Ver">1.5</xsl:variable>
    <xsl:variable name="Rev">1</xsl:variable>
	 
 	<!-- call add attr on root -->
    <xsl:template mode="add-atts" match="/MediaStorePortResource">
        <!-- call add common attr - call-template addAttrs_rootEL to set serviceVer-->
		<xsl:call-template name="addAttrs_rootEL"/> 
    </xsl:template>
 



    <!-- DELETE ELEMENTS MediaStorePortResource ( were present in 'objectXml_1.0' but removed from 'objectXml_1.3') -->

    <!--  moved to 
        /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceFileNID
        /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceNID -->
    <xsl:template match="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceFileNID"/>
    <xsl:template match="/MediaStorePortResource/Info/MediaTypeList"/>
    <xsl:template match="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/StreamNID"/>
    <xsl:template match="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/RoomNID"/>
    <xsl:template match="/MediaStorePortResource/Contact/Reachable"/>


    <!-- Add ELEMENTS MediaStorePortResource ( were absent in 'objectXml_1.0' but added to  'objectXml_1.3') -->

    <!--
        /MediaStorePortResource/Info/MediaStoreDeviceElementList /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceNID
        /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/MediaStreamInfo /MediaStorePortResource/Contact/Presence /MediaStorePortResource/ProfileInfo
        /MediaStorePortResource/Info/StreamTypeList/StreamURLType
    -->

    <xsl:template mode="append" match="/MediaStorePortResource/Info">

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
                <Type>Both</Type>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:choose>
            <xsl:when test="MediaStoreDeviceElementList">
            </xsl:when>
            <xsl:otherwise>
                <MediaStoreDeviceElementList>
                    <MediaStoreDeviceElementID></MediaStoreDeviceElementID>
                </MediaStoreDeviceElementList>
            </xsl:otherwise>
        </xsl:choose>


    </xsl:template>

    <xsl:template mode="append" match="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance">
        <!--
            moved to /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceFileNID /MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceNID
        -->
        <xsl:choose>
            <!-- new child  exist : do nothing -->
            <xsl:when test="ResourceNID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <!-- original child exist : get value into ResourceNID -->
                    <xsl:when test="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceFileNID">
                        <ResourceNID>
                            <xsl:value-of select="/MediaStorePortResource/InstanceInfo/InstanceList/ResourceInstance/ResourceFileNID"/>
                        </ResourceNID>
                    </xsl:when>
                    <xsl:otherwise>
                        <ResourceNID/>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>


        <xsl:choose>
            <xsl:when test="MediaStreamInfo">
            </xsl:when>
            <xsl:otherwise>
                <MediaStreamInfo>
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
                </MediaStreamInfo>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template mode="append" match="/MediaStorePortResource/Contact">

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

    <xsl:template mode="append" match="/MediaStorePortResource/State">

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

    <xsl:template mode="append" match="/MediaStorePortResource">
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

    <xsl:template mode="append" match="/MediaStorePortResource/Info/StreamTypeList">
        <xsl:choose>
            <xsl:when test="StreamURLType">
            </xsl:when>
            <xsl:otherwise>
                <StreamURLType/>
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

    <xsl:template match="//DeviceElementID">
        <DeviceElementID>0</DeviceElementID>
    </xsl:template>



</xsl:stylesheet>