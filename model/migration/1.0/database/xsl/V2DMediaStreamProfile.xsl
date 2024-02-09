<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- _dbtemplate.xsl :  include xsl template-->
    <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
	<!--_common.xsl : include xsl template -->
    <xsl:import href="_common.xsl"/>
   
	<!--  define variables  -->
    <xsl:variable name="Ver">1.3</xsl:variable>
    <xsl:variable name="Rev">1</xsl:variable>
	 
 	<!-- call add attr on root -->
    <xsl:template mode="add-atts" match="/V2DMediaStreamProfile">
        <!-- call add common attr - call-template addAttrs_rootEL to set serviceVer-->
		<xsl:call-template name="addAttrs_rootEL"/> 
    </xsl:template>


    <!--DELETE ELEMENTS V2DMediaStreamProfile ( were present in		'objectXml_1.0' but removed from 'objectXml_1.3')-->
    <!--move /V2DMediaStreamProfile/V2DStreamConfig to /V2DMediaStreamProfile/info-->
    <xsl:template match="/V2DMediaStreamProfile/V2DStreamConfig"/>
    <!--move /V2DMediaStreamProfile/V2DEncoderAudioConfig to /V2DMediaStreamProfile/info-->
    <xsl:template match="/V2DMediaStreamProfile/V2DEncoderAudioConfig"/>
    <!--move /V2DMediaStreamProfile/V2DEncoderConnectionParameterConfig to info-->
    <xsl:template match="/V2DMediaStreamProfile/V2DEncoderConnectionParameterConfig"/>
    <!--move /V2DMediaStreamProfile/V2DEncoderConnectionParameterKbmConfig to info-->
    <xsl:template match="/V2DMediaStreamProfile/V2DEncoderConnectionParameterKbmConfig"/>


    <!--Add ELEMENTS V2DMediaStreamProfile ( were absent in 'objectXml_1.0'but added to 'objectXml_1.3')-->

    <!-- /V2DMediaStreamProfile/Info-->

    <xsl:template mode="append" match="/V2DMediaStreamProfile">
        <xsl:choose>
            <xsl:when test="Info">
            </xsl:when>
            <xsl:otherwise>
                <Info>
                    <!-- move /V2DMediaStreamProfile/V2DStreamConfig into
						/V2DMediaStreamProfile/Info/ -->
                    <xsl:choose>
                        <!-- new child exist : do nothing -->
                        <xsl:when test="/V2DMediaStreamProfile/Info/V2DStreamConfig">
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:choose>
                                <!-- original child exist : copy else copy the default from objectxml -->
                                <xsl:when test="/V2DMediaStreamProfile/V2DStreamConfig">
                                    <xsl:copy-of select="/V2DMediaStreamProfile/V2DStreamConfig"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <V2DStreamConfig>
                                        <IsMulticast>false</IsMulticast>
                                        <EnableSRDTranscode>true</EnableSRDTranscode>
                                        <SRDTranscodeBandwidth></SRDTranscodeBandwidth>
                                        <AVOption>Both</AVOption>
                                    </V2DStreamConfig>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:otherwise>
                    </xsl:choose>

                    <!-- move /V2DMediaStreamProfile/V2DEncoderAudioConfig into /V2DMediaStreamProfile/Info -->
                    <xsl:choose>
                        <!-- new child exist : do nothing -->
                        <xsl:when test="/V2DMediaStreamProfile/Info/V2DEncoderAudioConfig">
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:choose>
                                <!-- original child exist : copy else copy the default from objectxml -->
                                <xsl:when test="/V2DMediaStreamProfile/V2DEncoderAudioConfig">
                                    <xsl:copy-of select="/V2DMediaStreamProfile/V2DEncoderAudioConfig"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <V2DEncoderAudioConfig>
                                        <SampleRate></SampleRate>
                                        <SampleSize></SampleSize>
                                        <MonoStereo></MonoStereo>
                                        <AudioEncodeEnable></AudioEncodeEnable>
                                    </V2DEncoderAudioConfig>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:otherwise>
                    </xsl:choose>


                    <!-- move /V2DMediaStreamProfile/V2DEncoderConnectionParameterConfig into /V2DMediaStreamProfile/Info -->
                    <xsl:choose>
                        <!-- new child exist : do nothing -->
                        <xsl:when test="/V2DMediaStreamProfile/Info/V2DEncoderConnectionParameterConfig">
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:choose>
                                <!-- original child exist : copy else copy the default from objectxml -->
                                <xsl:when test="/V2DMediaStreamProfile/V2DEncoderConnectionParameterConfig">
                                    <xsl:copy-of select="/V2DMediaStreamProfile/V2DEncoderConnectionParameterConfig"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <V2DEncoderConnectionParameterConfig>
                                        <BurstRatio>1</BurstRatio>
                                        <BurstDuration>50</BurstDuration>

                                        <Compression>
                                            <High>6</High>
                                            <Low>2</Low>
                                        </Compression>

                                        <Bandwidth>10.0</Bandwidth>

                                        <Refresh>
                                            <Min>1</Min>
                                            <Max>15</Max>
                                        </Refresh>

                                        <Slice>
                                            <Min>0</Min>
                                            <Max>15</Max>
                                        </Slice>

                                        <BlockThreshold>5</BlockThreshold>
                                        <Enable444>false</Enable444>
                                        <FrameRateDivider>1</FrameRateDivider>
                                    </V2DEncoderConnectionParameterConfig>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:otherwise>
                    </xsl:choose>


                    <!-- move /V2DMediaStreamProfile/V2DEncoderConnectionParameterKbmConfig into /V2DMediaStreamProfile/Info -->
                    <xsl:choose>
                        <!-- new child exist : do nothing -->
                        <xsl:when test="/V2DMediaStreamProfile/Info/V2DEncoderAudioConfig">
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:choose>
                                <!-- original child exist : copy else copy the default from objectxml -->
                                <xsl:when test="/V2DMediaStreamProfile/V2DEncoderConnectionParameterKbmConfig">
                                    <xsl:copy-of select="/V2DMediaStreamProfile/V2DEncoderConnectionParameterKbmConfig"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <V2DEncoderConnectionParameterKbmConfig>
                                        <AllowKMSwitch>true</AllowKMSwitch>
                                        <KMIdleTime>5</KMIdleTime>
                                        <EnableKbm>true</EnableKbm>
                                    </V2DEncoderConnectionParameterKbmConfig>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:otherwise>
                    </xsl:choose>

                </Info>
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


</xsl:stylesheet>