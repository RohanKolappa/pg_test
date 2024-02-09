<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>    
    <xsl:template match="//@from">
        <xsl:attribute name="from">
			<xsl:value-of select="$jid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//DeviceLoginRequestData/@type">
        <xsl:attribute name="type">
			<xsl:value-of select="$devicetype"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>
    
    
    <xsl:template match="//@serviceVer">
        <xsl:attribute name="serviceVer">
			<xsl:value-of select="$serviceversion"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>
    <xsl:template match="//@serviceVersion">
        <xsl:attribute name="serviceVersion">
			<xsl:value-of select="$serviceversion"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@softwareVersion">
        <xsl:attribute name="softwareVersion">
			<xsl:value-of select="$version"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//DevicePrivateKeyData">
        <xsl:copy>
			<xsl:value-of select="$password"></xsl:value-of>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="//DeviceAdminServiceInfo">
        <xsl:copy>
			<xsl:value-of select="$password"></xsl:value-of>
        </xsl:copy>
    </xsl:template>
    <xsl:template match="//LayoutServiceInfo">
	<xsl:choose>
		<xsl:when test='"StreamDst"=$portinfo'>			
			<xsl:copy>
				<xsl:copy-of select="document($streaminfolist)//StreamInfoList"/>
			</xsl:copy>
		</xsl:when>
	</xsl:choose>
    </xsl:template>
     <xsl:template match="//MediaServiceInfo">
	<xsl:choose>
		<xsl:when test='"StreamSrc"=$portinfo'>
			<xsl:copy>
				<xsl:copy-of select="document($streaminfolist)//StreamInfoList"/>
			</xsl:copy>
		</xsl:when>
	</xsl:choose>
    </xsl:template>
    <xsl:template match="//DeviceAdminServiceInfo">
		<xsl:copy>
		<Port type="NetworkPort" id="1" ready="" maxInstanceCount="">
			<NetworkStatus>
			<xsl:copy-of select="document($statusfile)//Parameters"/>
			</NetworkStatus>
		</Port>
		<xsl:choose>
			<xsl:when test='"StreamSrc"=$portinfo'>
				<Port type="SrcPort" id="1" streamType="V2D"/>
			</xsl:when>
			<xsl:otherwise>
				<Port type="DstPort" id="1" streamType="V2D"/>
			</xsl:otherwise>
		</xsl:choose>
		</xsl:copy>
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
</xsl:stylesheet>

