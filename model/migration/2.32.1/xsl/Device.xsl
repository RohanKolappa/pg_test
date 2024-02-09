<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">
		<xsl:text>2.32.2</xsl:text>
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
            <xsl:when test='"2.32.2"=$objectXml_serviceVer'>
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


	<xsl:template match="//HardDiskEntry" mode="append">
		<xsl:if
			test="//MS-Device or //MS-25-Device or //DMS-25-Device or 
	 					//DMS-100-Device or //DMS-120-Device or //DMS-200-Device or 
	 					//DMS-1000-Device or //DMS-1200-Device or 
	 					//DMS-1600-Device or //DMS-1700-Device">

			<SystemStore>
				<SystemReservation Size="5368709120">
					<RootDirPath>/data1</RootDirPath>
				</SystemReservation>
				<ThumbnailReservation Size="5368709120">
					<RootDirPath>/data1/tncache/</RootDirPath>
				</ThumbnailReservation>
			</SystemStore>
		</xsl:if>
	</xsl:template>

	<xsl:template match="//DMS-1500-Device//HardDiskEntry"mode="append">
		<xsl:choose>
			<xsl:when test="./MountPoint='/data1'">
				<SystemStore>
					<SystemReservation Size="5368709120">
						<RootDirPath>/data1</RootDirPath>
					</SystemReservation>
					<ThumbnailReservation Size="5368709120">
						<RootDirPath>/data1/tncache/</RootDirPath>
					</ThumbnailReservation>
				</SystemStore>
			</xsl:when>
			<xsl:otherwise>
				<SystemStore>
					<SystemReservation Size="5368709120">
						<RootDirPath>/data2</RootDirPath>
					</SystemReservation>
					<ThumbnailReservation Size="0">
						<RootDirPath>/data2/tncache/</RootDirPath>
					</ThumbnailReservation>
				</SystemStore>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="//NASEntry" mode="append">
		<xsl:if
			test="//DMS-25-Device or
                  //DMS-100-Device or //DMS-120-Device or //DMS-200-Device or
                  //DMS-1000-Device or //DMS-1200-Device or //DMS-1500-Device or
                  //DMS-1600-Device or //DMS-1700-Device">

			<SystemStore>
				<SystemReservation Size="5368709120">
					<RootDirPath>/data/DSS/data/portal/site/NAS</RootDirPath>
				</SystemReservation>
				<ThumbnailReservation Size="0">
					<RootDirPath>/data/DSS/data/portal/site/NAS</RootDirPath>
				</ThumbnailReservation>
			</SystemStore>

		</xsl:if>
	</xsl:template>    
	
</xsl:stylesheet>

