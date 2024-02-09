<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.16</xsl:text>
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
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"2.16"=$objectXml_serviceVer'>
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


   <xsl:template match="HostName">
        <xsl:choose>
            <xsl:when test='string-length(normalize-space(.))=0'>
				<xsl:variable name="hostname">
					<xsl:call-template name="GetHostName">
						<xsl:with-param name="devname"><xsl:value-of select="name(/*)"/></xsl:with-param>
					</xsl:call-template>
				</xsl:variable>
			<HostName><xsl:value-of select="normalize-space($hostname)"/></HostName>
            </xsl:when>
            <xsl:otherwise><xsl:copy-of select="."/></xsl:otherwise></xsl:choose>
  </xsl:template>

  <xsl:template name="GetHostName">
        <xsl:param name="devname" />
        <xsl:choose>
		<xsl:when test="$devname='DMS-1000-Device'">dms1000</xsl:when> 
		<xsl:when test="$devname='DMS-100-Device'">dms100</xsl:when> 
		<xsl:when test="$devname='DMS-1200-Device'">dms1200</xsl:when> 
		<xsl:when test="$devname='DMS-120-Device'">dms120</xsl:when> 
		<xsl:when test="$devname='DMS-1500-Device'">dms1500</xsl:when> 
		<xsl:when test="$devname='DMS-200-Device'">dms200</xsl:when> 
		<xsl:when test="$devname='MS-Device'">ms</xsl:when>
		<xsl:when test="$devname='V2D-Rx-Device'">v2drx</xsl:when>
		<xsl:when test="$devname='V2D-Tx-Device'">v2dtx</xsl:when>
		<xsl:when test="$devname='V2D-XP100-Device'">v2dxp100</xsl:when>
		<xsl:when test="$devname='V2D-XP200-Device'">v2dxp200</xsl:when>
		<xsl:when test="$devname='V2D-XP200S-Device'">v2dxp200s</xsl:when>
		<xsl:when test="$devname='V2D-XP220-Device'">v2dxp220</xsl:when>
		<xsl:when test="$devname='V2D-XP220S-Device'">v2dxp220s</xsl:when> 
		<xsl:otherwise></xsl:otherwise>
        </xsl:choose>
  </xsl:template>    
</xsl:stylesheet>

