
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    <xsl:template match="/">
        <IPVSClient-Device serviceVer="" Ver="1.5" Rev="1" parentNID="default.devicelist">
            <xsl:variable name="assetnid">
                <xsl:value-of select="/AssetResource/@NID"/>
            </xsl:variable>
            <xsl:attribute name="NID">default.devicelist.dl_<xsl:value-of select="substring-after($assetnid,'assetresourcelist.')"/></xsl:attribute>
            <AssetResourceNID>
                <xsl:value-of select="$assetnid"/>
            </AssetResourceNID>
            <ManagementMode>Local</ManagementMode>

            <ResourcePointers>
                <MediaStreamDstPortResourceNID>
                    <NIDValue>default.mediastreamdstportresourcelist.msdprl_<xsl:value-of select="substring-after($assetnid,'assetresourcelist.')"/></NIDValue>
                </MediaStreamDstPortResourceNID>
            </ResourcePointers>

            <DeviceConfig Rev="1" Synched="false">
                <SystemElementsConfig>
                    <SoftwareImageConfig Dirty="CLEAN">
                        <RootPassword></RootPassword>
                        <NewImageName></NewImageName>
                        <NewVersion></NewVersion>
                    </SoftwareImageConfig>

                    <LicenseConfig Dirty="CLEAN">
                        <NewLicenseKey></NewLicenseKey>
                    </LicenseConfig>

                    <AdminServiceConfig Dirty="CLEAN">
                        <Password>admin</Password>
                    </AdminServiceConfig>
                </SystemElementsConfig>

                <ServiceElementsConfig>
                    <XMPPAgentServiceConfig Dirty="CLEAN">
                        <Enable>false</Enable>
                        <XMPPServer>127.0.0.1</XMPPServer>
                        <XMPPServerPort>5222</XMPPServerPort>
                        <User>managementserver</User>
                        <Domain>string</Domain>
                        <Password>msadmin</Password>
                        <AgentID>managementserver</AgentID>
                        <ServiceDomain>default</ServiceDomain>

                        <AgentList>
                            <XMPPAgentServiceAdminAgentConfig Dirty="CLEAN">
                                <Enable>true</Enable>
                            </XMPPAgentServiceAdminAgentConfig>

                            <XMPPAgentServiceConfigAgentConfig Dirty="CLEAN">
                                <Enable>true</Enable>
                            </XMPPAgentServiceConfigAgentConfig>

                            <XMPPAgentServiceDestinationAgentConfig Dirty="CLEAN">
                                <Enable>true</Enable>
                            </XMPPAgentServiceDestinationAgentConfig>

                            <XMPPAgentServiceXAuthAgentConfig Dirty="CLEAN">
                                <Enable>true</Enable>
                                <XAuthServer></XAuthServer>
                                <XAuthServerPort></XAuthServerPort>
                                <XAuthKeepAliveInterval></XAuthKeepAliveInterval>
                                <XAuthKeepAliveTimeout></XAuthKeepAliveTimeout>
                            </XMPPAgentServiceXAuthAgentConfig>
                        </AgentList>
                    </XMPPAgentServiceConfig>

                    <StreamingServiceConfig Dirty="CLEAN">
                        <MaxStreamInstanceCount></MaxStreamInstanceCount>

                        <V2DStreamsRxConfig>
                            <MaxStreamInstanceCount></MaxStreamInstanceCount>

                            <RxMediaPortRange>
                                <From>6060</From>
                                <To>7000</To>
                            </RxMediaPortRange>
                        </V2DStreamsRxConfig>

                        <MPEGTSStreamsRxConfig>
                            <MaxStreamInstanceCount></MaxStreamInstanceCount>

                            <RxMediaPortRange>
                                <From>6060</From>
                                <To>7000</To>
                            </RxMediaPortRange>
                        </MPEGTSStreamsRxConfig>

                        <RTPStreamsRxConfig>
                            <MaxStreamInstanceCount></MaxStreamInstanceCount>

                            <RxMediaPortRange>
                                <From>6060</From>
                                <To>7000</To>
                            </RxMediaPortRange>
                        </RTPStreamsRxConfig>

                        <V2DStreamsDecodeConfig>
                            <MaxStreamInstanceCount></MaxStreamInstanceCount>
                        </V2DStreamsDecodeConfig>

                        <MPEGStreamsDecodeConfig>
                            <MaxStreamInstanceCount></MaxStreamInstanceCount>
                        </MPEGStreamsDecodeConfig>
                    </StreamingServiceConfig>
                </ServiceElementsConfig>

                <NetworkElementsConfig>
                    <NATTraversalTableConfig></NATTraversalTableConfig>

                    <EthernetPortTableConfig Dirty="CLEAN">
                        <EthernetPortConfig Dirty="CLEAN" ListIndex="1">
                            <PortID>1</PortID>
                            <EnableInterface>false</EnableInterface>
                            <IsPrimary>false</IsPrimary>
                            <EnableMulticast>false</EnableMulticast>

                            <IPConfig>
                                <UseDHCP>false</UseDHCP>
                                <IPAddress></IPAddress>
                                <Netmask></Netmask>
                            </IPConfig>
                        </EthernetPortConfig>
                    </EthernetPortTableConfig>
                </NetworkElementsConfig>

                <AVCodecElementsConfig>
                    <V2DSoftDecoderConfig Dirty="CLEAN">
                        <PortID></PortID>
                        <MaxElementInstanceCount></MaxElementInstanceCount>

                        <Info>
                            <StreamType></StreamType>
                        </Info>
                    </V2DSoftDecoderConfig>

                    <MPEGDecoderConfig Dirty="CLEAN">
                        <PortID></PortID>
                        <MaxElementInstanceCount></MaxElementInstanceCount>

                        <Info>
                            <StreamType></StreamType>
                            <StreamType></StreamType>
                            <Platform></Platform>

                            <MPEGDecoderParameterConfig>
                                <Parameter Name="" Value=""></Parameter>
                            </MPEGDecoderParameterConfig>
                        </Info>
                    </MPEGDecoderConfig>
                </AVCodecElementsConfig>
            </DeviceConfig>

            <DeviceStatus>
                <SystemElementsStatus>
                    <SoftwareImageStatus Queryable="true">
                        <Parameters>
                            <Parameter name="" type="Float" units="" source=""></Parameter>
                        </Parameters>
                    </SoftwareImageStatus>

                    <LicenseStatus>
                        <LicenseInfo>
                            <ProductID></ProductID>
                            <Company></Company>
                            <Key></Key>
                        </LicenseInfo>

                        <LicensePack>
                            <Name></Name>
                            <ValidUntil></ValidUntil>
                            <Value></Value>
                            <Key></Key>
                        </LicensePack>
                    </LicenseStatus>
                </SystemElementsStatus>

                <ServiceElementsStatus>
                    <XMPPAgentServiceStatus Queryable="true">
                        <Parameters>
                            <Parameter name="" type="Float" units="" source=""></Parameter>
                        </Parameters>

                        <AgentList>
                            <XMPPAgentServiceAdminAgentStatus Queryable="true">
                                <Parameters>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </Parameters>
                            </XMPPAgentServiceAdminAgentStatus>

                            <XMPPAgentServiceConfigAgentStatus Queryable="true">
                                <Parameters>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </Parameters>
                            </XMPPAgentServiceConfigAgentStatus>

                            <XMPPAgentServiceDestinationAgentStatus Queryable="true">
                                <Parameters>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </Parameters>
                            </XMPPAgentServiceDestinationAgentStatus>

                            <XMPPAgentServiceXAuthAgentStatus Queryable="true">
                                <Parameters>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </Parameters>
                            </XMPPAgentServiceXAuthAgentStatus>
                        </AgentList>
                    </XMPPAgentServiceStatus>

                    <StreamingServiceStatus>
                        <CurrentStreamInstanceCount>
                            <Total></Total>
                            <Transmit></Transmit>
                            <Receive></Receive>
                            <Read></Read>
                            <Write></Write>
                            <Encode></Encode>
                            <Decode></Decode>
                        </CurrentStreamInstanceCount>

                        <InstanceList>
                            <V2DStreamRxInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <V2DStreamRxInstanceConfig>
                                    <TxCtrlIpAddress></TxCtrlIpAddress>
                                    <MulticastTTL></MulticastTTL>
                                    <MulticastIpAddress></MulticastIpAddress>
                                    <RxMediaIpAddress></RxMediaIpAddress>
                                    <RxMediaPort></RxMediaPort>
                                    <TxCtrlPort></TxCtrlPort>

                                    <V2DStreamConfig>
                                        <IsMulticast></IsMulticast>
                                        <EnableSRDTranscode></EnableSRDTranscode>
                                        <SRDTranscodeBandwidth></SRDTranscodeBandwidth>
                                        <AVOption></AVOption>
                                    </V2DStreamConfig>
                                </V2DStreamRxInstanceConfig>
                            </V2DStreamRxInstance>

                            <MPEGTSStreamRxInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <MPEGTSStreamRxInstanceConfig>
                                    <TxCtrlIpAddress></TxCtrlIpAddress>
                                    <MulticastTTL></MulticastTTL>
                                    <MulticastIpAddress></MulticastIpAddress>
                                    <RxMediaIpAddress></RxMediaIpAddress>
                                    <RxMediaPort></RxMediaPort>

                                    <MPEGTSStreamConfig>
                                        <IsMulticast>true</IsMulticast>
                                    </MPEGTSStreamConfig>
                                </MPEGTSStreamRxInstanceConfig>
                            </MPEGTSStreamRxInstance>

                            <RTPStreamRxInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <RTPStreamRxInstanceConfig>
                                    <TxCtrlIpAddress></TxCtrlIpAddress>
                                    <MulticastTTL></MulticastTTL>
                                    <MulticastIpAddress></MulticastIpAddress>
                                    <RxMediaIpAddress></RxMediaIpAddress>
                                    <RxMediaPort></RxMediaPort>

                                    <RTPStreamConfig>
                                        <SDP-m-Template></SDP-m-Template>
                                        <IsMulticast></IsMulticast>
                                        <AVOption></AVOption>
                                    </RTPStreamConfig>

                                    <SDP></SDP>
                                </RTPStreamRxInstanceConfig>
                            </RTPStreamRxInstance>

                            <StreamReaderInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <StreamReaderInstanceConfig>
                                    <StartOffset></StartOffset>
                                    <FFTrackReadSelection></FFTrackReadSelection>
                                </StreamReaderInstanceConfig>
                            </StreamReaderInstance>

                            <V2DDecoderInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <V2DDecoderInstanceConfig>
                                    <V2DDecoderInstanceConnectionEntryConfig>
                                        <IPAddress></IPAddress>
                                        <Port></Port>
                                        <ConnectionID></ConnectionID>
                                        <TransmitterName></TransmitterName>
                                        <Bandwidth></Bandwidth>
                                        <IsMulticast></IsMulticast>
                                        <AVOption></AVOption>
                                        <SampleRate></SampleRate>
                                        <SampleSize></SampleSize>
                                        <MonoStereo></MonoStereo>
                                        <AllowRemoteKBMSControl></AllowRemoteKBMSControl>
                                    </V2DDecoderInstanceConnectionEntryConfig>
                                </V2DDecoderInstanceConfig>
                            </V2DDecoderInstance>

                            <MPEGDecoderInstance>
                                <SSInstanceID></SSInstanceID>
                                <QID></QID>
                                <ReferenceCount></ReferenceCount>

                                <InstanceStatus>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </InstanceStatus>

                                <MPEGDecoderInstanceConfig>
                                    <MPEGDecoderParameterConfig>
                                        <Parameter Name="" Value=""></Parameter>
                                    </MPEGDecoderParameterConfig>
                                </MPEGDecoderInstanceConfig>
                            </MPEGDecoderInstance>
                        </InstanceList>
                    </StreamingServiceStatus>
                </ServiceElementsStatus>

                <NetworkElementsStatus>
                    <NATTraversalTableStatus></NATTraversalTableStatus>

                    <EthernetPortTableStatus Queryable="true">
                        <EthernetPortStatus Queryable="true">
                            <ParametersList>
                                <Parameters>
                                    <Parameter name="" type="Float" units="" source=""></Parameter>
                                </Parameters>
                            </ParametersList>
                        </EthernetPortStatus>
                    </EthernetPortTableStatus>
                </NetworkElementsStatus>

                <AVCodecElementsStatus>
                    <V2DSoftDecoderStatus>
                        <PortID></PortID>

                        <InstanceInfo>
                            <PortInstance>
                                <ID></ID>
                            </PortInstance>
                        </InstanceInfo>
                    </V2DSoftDecoderStatus>

                    <SoftDecoderStatus>
                        <PortID></PortID>

                        <InstanceInfo>
                            <PortInstance>
                                <ID></ID>
                            </PortInstance>
                        </InstanceInfo>
                    </SoftDecoderStatus>
                </AVCodecElementsStatus>
            </DeviceStatus>
        </IPVSClient-Device>
    </xsl:template>







</xsl:stylesheet>