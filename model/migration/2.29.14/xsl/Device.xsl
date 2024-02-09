<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.29.15</xsl:text>
	</xsl:attribute>
    </xsl:template>
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
            <xsl:when test='"2.29.15"=$objectXml_serviceVer'>
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

    
</xsl:stylesheet>

