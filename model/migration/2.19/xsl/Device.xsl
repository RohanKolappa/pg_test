<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.21</xsl:text>
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

    <xsl:template match="XMPPServiceConfig/HAServer"/>
    <xsl:template match="XMPPServiceConfig/DNSServer"/>
    <xsl:template match="XMPPServiceStatus/HAServerStatus"/>
    <xsl:template match="XMPPServiceStatus/DNSServerStatus"/>


    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.21"=$objectXml_serviceVer'>
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
	 <xsl:template mode="append" match="/MS-Device//ServiceElementsConfig | /MS-25-Device//ServiceElementsConfig">
		<xsl:choose>
			<xsl:when test='HAServiceConfig'>
			</xsl:when>
			<xsl:otherwise>
				<HAServiceConfig>
					<xsl:choose>
						<xsl:when test='XMPPServiceConfig/HAServer'>
							<xsl:copy-of select="XMPPServiceConfig/HAServer"></xsl:copy-of>
						</xsl:when>
						<xsl:otherwise>
							<HAServer>
								<HAServerEnable></HAServerEnable>
								<DeadTime></DeadTime>
								<PeerIP></PeerIP>
								<PeerName></PeerName>
								<Port></Port>
								<VirtualIPEnable></VirtualIPEnable>
								<VirtualIP></VirtualIP>
								<Primary></Primary>
							</HAServer>
						</xsl:otherwise>
					</xsl:choose>
					<HADNSServer>
						<DNSServerEnable></DNSServerEnable>
					</HADNSServer>
				</HAServiceConfig>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	 <xsl:template mode="append" match="/MS-Device//DeviceCommand | /MS-25-Device//DeviceCommand">
		<xsl:choose>
			<xsl:when test='HATakeoverCommand'>
			</xsl:when>
			<xsl:otherwise>
				<HATakeoverCommand/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
</xsl:stylesheet>

