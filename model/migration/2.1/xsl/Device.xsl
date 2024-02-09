<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>

    <xsl:param name="devname">
    </xsl:param>
    <xsl:variable name="dms100" select="'DMS-100-Device'"/>
    <xsl:variable name="mgmnt">
        <xsl:value-of select="//XMPPServerEnable"/>
    </xsl:variable>

    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.2</xsl:text>
	</xsl:attribute>
    </xsl:template>
    <!-- replace PortID in XP200 and V2D to use 1,2 instead of 0,1 -->
    <xsl:template match="//V2DDecoderConfig[PortID='0']/PortID">
        <PortID>1</PortID>
    </xsl:template>
    <xsl:template match="//V2DEncoderConfig[PortID='0']/PortID">
        <PortID>1</PortID>
    </xsl:template>
    <xsl:template match="//XP200PortConfig[PortID='0']/PortID">
        <PortID>1</PortID>
    </xsl:template>
    <xsl:template match="/V2D-XP200-Device//XP200PortConfig[PortID='1']/PortID">
        <PortID>2</PortID>
    </xsl:template>
  <xsl:template match="//LUTID">
	<LUTID><xsl:value-of select="../@ListIndex"/></LUTID>
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
            <xsl:when test='"2.2"=$objectXml_serviceVer'>
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



</xsl:stylesheet>



