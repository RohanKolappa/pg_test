<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>

    <xsl:param name="devname">
    </xsl:param>
    <xsl:variable name="dms100" select="'DMS-100-Device'"/>
    <xsl:variable name="mgmnt">
        <xsl:value-of select="//XMPPServerEnable"/>
    </xsl:variable>

    <xsl:template match="/*">
        <xsl:choose>
            <xsl:when test='$devname=$dms100'>
                <xsl:choose>
                    <xsl:when test='"false"=$mgmnt'>
                        <xsl:element name="{concat ('', $devname)}">
                            <xsl:apply-templates select="@*|node()"/>
                        </xsl:element>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:element name="MS-Device">
                            <xsl:apply-templates select="@*|node()"/>
                        </xsl:element>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <xsl:element name="{concat ('', $devname)}">
                    <xsl:apply-templates select="@*|node()"/>
                </xsl:element>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="/*/@Version | /*/@Ver">
        <xsl:attribute name="Ver">
		  <xsl:text>1.5</xsl:text>
	    </xsl:attribute>
        <xsl:attribute name="serviceVer">
		  <xsl:text>1.3</xsl:text>
	    </xsl:attribute>
    </xsl:template>

    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		  <xsl:text>1.3</xsl:text>
	    </xsl:attribute>
    </xsl:template>

    <!-- Changes for XMPP Service -->
    <xsl:template match="XMPPServiceConfig">
        <XMPPServiceConfig Dirty="CLEAN">
            <XMPPServer>
                <XMPPServerEnable>
                    <xsl:value-of select="//XMPPServerEnable"/>
                </XMPPServerEnable>
                <XMPPServerPort>5222</XMPPServerPort>
                <ActiveDirectory>
                    <XMPPServerEnableADS>false</XMPPServerEnableADS>
                    <XMPPServerADSHostName>0.0.0.0</XMPPServerADSHostName>
                </ActiveDirectory>
            </XMPPServer>
            <IPVSAuthService>
                <SuperUserDefaults>
                    <UserName/>
                    <Password/>
                </SuperUserDefaults>
            </IPVSAuthService>
            <IPVSDBService>
                <Backup>
                    <DailyBackup>true</DailyBackup>
                    <BackupTime>0</BackupTime>
                    <EnableExport>false</EnableExport>
                    <ExportURL/>
                </Backup>
                <Restore>
                    <RecoveryMode>LastBackup</RecoveryMode>
                </Restore>
            </IPVSDBService>
        </XMPPServiceConfig>
    </xsl:template>


    <xsl:template match="SerialPort">
        <Enable>false</Enable>
    </xsl:template>

    <!-- Add content only after <DNSClientConfig/> -->
    <!-- If EthernetPort Table not present      /> -->
    <xsl:template mode="after" match="DNSClientConfig">
        <xsl:choose>
            <xsl:when test="//EthernetPortTableConfig">
            </xsl:when>
            <xsl:otherwise>
                <EthernetPortTableConfig>
                    <EthernetPortConfig ListIndex="1" Dirty="CLEAN">
                        <PortID>1</PortID>
                        <IsPrimary>true</IsPrimary>
                        <EnableInterface>
                            <xsl:value-of select="//PrimaryEthernetPortConfig/IPConfig/EnableInterface"/>
                        </EnableInterface>
                        <EnableMulticast>true</EnableMulticast>
                        <xsl:copy-of select="//PrimaryEthernetPortConfig/IPConfig"/>
                    </EthernetPortConfig>
                    <xsl:for-each select="//SecondaryEthernetPortConfig">
                        <EthernetPortConfig ListIndex="2" Dirty="CLEAN">
                            <PortID>2</PortID>
                            <IsPrimary>false</IsPrimary>
                            <EnableInterface>
                                <xsl:value-of select="//SecondaryEthernetPortConfig/IPConfig/EnableInterface"/>
                            </EnableInterface>
                            <EnableMulticast>true</EnableMulticast>
                            <xsl:copy-of select="//SecondaryEthernetPortConfig/IPConfig"/>
                        </EthernetPortConfig>
                    </xsl:for-each>
                    <xsl:for-each select="//AuxiliaryEthernetPortConfig">
                        <EthernetPortConfig ListIndex="3" Dirty="CLEAN">
                            <PortID>3</PortID>
                            <IsPrimary>false</IsPrimary>
                            <EnableInterface>
                                <xsl:value-of select="//AuxiliaryEthernetPortConfig/IPConfig/EnableInterface"/>
                            </EnableInterface>
                            <EnableMulticast>true</EnableMulticast>
                            <xsl:copy-of select="//AuxiliaryEthernetPortConfig/IPConfig"/>
                        </EthernetPortConfig>
                    </xsl:for-each>
                </EthernetPortTableConfig>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="IPConfig/EnableInterface"/>
    <xsl:template match="PrimaryEthernetPortConfig"/>
    <xsl:template match="SecondaryEthernetPortConfig"/>
    <xsl:template match="AuxiliaryEthernetPortConfig"/>
    <xsl:template mode="after" match="RouteTableConfig">
        <HostTableConfig Dirty="CLEAN"/>
        <NATZoneTableConfig Dirty="CLEAN"/>
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

    <!--
        ////////// deleted DMS-1000-Device DEL_TODO <xsl:template match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/WISStreamsEncodeConfig"/> DEL_TODO <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/MPEGTSStreamsRxConfig/DefaultInstanceConfig"/> DEL_TODO <xsl:template
        match="/DMS-1000-Device/DeviceConfig/NetworkElementsConfig/RouteTableConfig/RouteEntry/RouteID"/> DEL_TODO <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MuticastTTL"/> DEL_VERIFY <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/V2DStreamsRxConfig/RxAVPortRange"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/NetworkElementsConfig/PrimaryEthernetPortConfig"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/NetworkElementsConfig/AuxiliaryEthernetPortConfig"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/XMPPServiceConfig/XMPPServerEnable"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/NetworkElementsConfig/SecondaryEthernetPortConfig"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/XMPPServiceConfig/XMPPServerPort"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/XMPPServiceConfig/XMPPServerADSHostName"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/ServiceElementsConfig/XMPPServiceConfig/XMPPServerEnableADS"/> DEL_DONE <xsl:template
        match="/DMS-1000-Device/DeviceConfig/StorageElementsConfig/HardDiskConfig/MediaStoreElementList"/> ///////// deleted V2D-Tx-Device DEL_TODO <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/PumpCurr"/> DEL_TODO <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/Offset"/> DEL_TODO <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/RouteTableConfig/RouteEntry/RouteID"/> DEL_TODO <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/UpdateCurrentLUT"/> DEL_TODO <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig/Gain"/> DEL_VERIFY <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig/SplashBandwidth"/> DEL_DONE <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/PrimaryEthernetPortConfig"/> DEL_DONE <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MuticastTTL"/> DEL_DONE <xsl:template
        match="/V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/SerialPort"/> ///////// new added DMS-1000-Device ADD_DONE
        /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/EthernetPortTableConfig ADD_DONE /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/HostTableConfig ADD_DONE
        /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/NATZoneTableConfig ADD_DONE /V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/Enable ADD_DONE
        /V2D-Tx-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MulticastTTL ?? /V2D-Tx-Device/DeviceCommand ///////// new added V2D-Tx-Device
        ADD_DONE /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/EthernetPortTableConfig ADD_DONE /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/HostTableConfig ADD_DONE
        /V2D-Tx-Device/DeviceConfig/NetworkElementsConfig/NATZoneTableConfig ADD_DONE /V2D-Tx-Device/DeviceConfig/AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig/Enable ADD_DONE
        /V2D-Tx-Device/DeviceConfig/ServiceElementsConfig/StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MulticastTTL
    -->

    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"1.3"=$objectXml_serviceVer'>
                <xsl:copy-of select="document($objectxml_file)//DeviceStatus"/>
            </xsl:when>
            <xsl:otherwise>
                <DeviceStatus>
                    skip copy
                    target serviceVer
                    <xsl:value-of select="$objectXml_serviceVer"/>
                    in objectXml
                    <xsl:value-of select="$objectxml_file"/>
                </DeviceStatus>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>




    <!--  deleted DMS-1000-Device -->
    <xsl:template match="//StreamingServiceConfig/WISStreamsEncodeConfig"/>
    <xsl:template match="//StreamingServiceConfig/MPEGTSStreamsRxConfig/DefaultInstanceConfig"/>
    <xsl:template match="//RouteTableConfig/RouteEntry/RouteID"/>
    <xsl:template match="//StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MuticastTTL">
        <MulticastTTL>
            <xsl:value-of select="//StreamingServiceConfig/StreamsTxConfig/MulticastIpAddressRange/MuticastTTL"/>
        </MulticastTTL>
    </xsl:template>
    <xsl:template match="//StreamingServiceConfig/V2DStreamsRxConfig/RxAVPortRange"/>

    <!--  deleted V2D-Tx-Device -->
    <xsl:template match="//V2DEncoderConfig/V2DEncoderVideoConfig/PumpCurr"/>
    <xsl:template match="//V2DEncoderConfig/V2DEncoderVideoConfig/Offset"/>
    <xsl:template match="//V2DEncoderConfig/V2DEncoderVideoConfig/UpdateCurrentLUT"/>
    <xsl:template match="//V2DEncoderConfig/V2DEncoderVideoConfig/Gain"/>
    <xsl:template match="//V2DEncoderConfig/V2DEncoderSplashParametersConfig/SplashBandwidth"/>


</xsl:stylesheet>

