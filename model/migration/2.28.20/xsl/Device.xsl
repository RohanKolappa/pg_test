<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.29.16</xsl:text>
	</xsl:attribute>
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


    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus[not(*)]">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.29.16"=$objectXml_serviceVer'>
                <xsl:copy-of select="document($objectxml_file)//DeviceStatus"/>
            </xsl:when>
            <xsl:otherwise>
                <DeviceStatus>
                    skip copy 
                    target serviceVer <xsl:value-of select="$objectXml_serviceVer"/>
                    in objectXml <xsl:value-of select="$objectxml_file"/>
                </DeviceStatus>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

	<!-- From 2.28.12 on 314 trunk  -->
	<xsl:template mode="after" match="//EthernetPortConfig/EnableMulticast">
			<MTU>1500</MTU>
	</xsl:template>
	
	<!-- From 2.29.4 on 314 trunk  -->
	<xsl:template mode="after" match="//AdminServiceConfig">
        <xsl:choose>
            <xsl:when test="//LoginBannerConfig">
            </xsl:when>
            <xsl:otherwise>
				<xsl:variable name="rootname" select="name(/*)"/>
				<xsl:variable name="objectxml_file">
					<xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
				</xsl:variable>
				<xsl:copy-of select="document($objectxml_file)//LoginBannerConfig"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- From 2.29.5 on 314 trunk  -->
    <xsl:template mode="after" match="//Override">
        <xsl:choose>
            <xsl:when test='//OverrideColor'>
			</xsl:when>
			<xsl:otherwise>
				<OverrideColor>Blue</OverrideColor>
			</xsl:otherwise>
        </xsl:choose>
	</xsl:template>
    
    <!-- From 2.29.12 on 314 trunk  -->
    <xsl:template mode="after" match="//PortIOType">
		<MaxEncoderInstanceCount>1</MaxEncoderInstanceCount>
    </xsl:template>
    <xsl:template mode="before" match="//V2DEncoderPortKbmConfig/KbType">
		<SubChannel>0</SubChannel>
    </xsl:template>
    <xsl:template mode="before" match="//V2DEncoderPortRFBConfig/EnableRFB">
		<SubChannel>0</SubChannel>
    </xsl:template>

    <xsl:template match="//DeviceStatus">
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
			<xsl:attribute name="State">
				<xsl:text>OK</xsl:text>
			</xsl:attribute>
			<xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="//DeviceStatus/*">
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
			<xsl:attribute name="State">
				<xsl:text>OK</xsl:text>
			</xsl:attribute>
			<xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="//XP200PortStatus">
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
			<xsl:attribute name="State">
				<xsl:text>OK</xsl:text>
			</xsl:attribute>
			<xsl:apply-templates/>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="//@Queryable">
        <xsl:attribute name="Queryable">
			<xsl:text>true</xsl:text>
		</xsl:attribute>
        <xsl:attribute name="State">
			<xsl:text>OK</xsl:text>
		</xsl:attribute>
    </xsl:template>
    
    <!-- From 2.29.14 on 314 trunk  -->
    <xsl:template mode="before" match="//XMPPAgentServiceConfig">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:choose>
        <xsl:when test='"V2D"=substring-before($rootname, "-") or "IPVSClient"=substring-before($rootname, "-") or "ORB"=substring-before($rootname, "-")'>
			<LinkLocalXMPPAgentServiceConfig Dirty="CLEAN">
				<Enable>true</Enable>
				<AgentList>
					<XMPPAgentServiceConfigAgentConfig Dirty="CLEAN">
						<Enable>true</Enable>
					</XMPPAgentServiceConfigAgentConfig>
				</AgentList>
			</LinkLocalXMPPAgentServiceConfig>
        </xsl:when>
        <xsl:when test='"DMS"=substring-before($rootname, "-") or "MS"=substring-before($rootname, "-")'>
			<LinkLocalXMPPAgentServiceConfig Dirty="CLEAN">
				<Enable>true</Enable>
				<AgentList>
					<XMPPAgentServiceAdminAgentConfig Dirty="CLEAN">
						<Enable>true</Enable>
					</XMPPAgentServiceAdminAgentConfig>
					<XMPPAgentServiceConfigAgentConfig Dirty="CLEAN">
						<Enable>true</Enable>
					</XMPPAgentServiceConfigAgentConfig>
				</AgentList>
			</LinkLocalXMPPAgentServiceConfig>
        </xsl:when>
        <xsl:when test='"IPVSClient"=substring-before($rootname, "-")'>
			<LinkLocalXMPPAgentServiceConfig Dirty="CLEAN">
				<Enable>true</Enable>
				<AgentList>
					<XMPPAgentServiceConfigAgentConfig Dirty="CLEAN">
						<Enable>true</Enable>
					</XMPPAgentServiceConfigAgentConfig>
				</AgentList>
			</LinkLocalXMPPAgentServiceConfig>
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    
</xsl:stylesheet>


