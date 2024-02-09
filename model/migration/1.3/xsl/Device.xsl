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
		<xsl:text>1.4</xsl:text>
	</xsl:attribute>
    </xsl:template>

    <xsl:template match="//HardDiskConfig">
        <HardDiskTableConfig Dirty="CLEAN">
            <HardDiskEntry Dirty="CLEAN" ListIndex="1">
                <DeviceName>/dev/sda1</DeviceName>
                <MountPoint>/data1</MountPoint>
                <RAIDArrayEntry>
                    <Controller>c0</Controller>
                    <Unit>u0</Unit>
                    <xsl:choose>
                        <xsl:when test="//DMS-100-Device">
                            <RAIDType>RAID-1</RAIDType>
                            <RAIDDiskTableConfig Dirty="CLEAN">
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="1">
                                    <Port>1</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="2">
                                    <Port>2</Port>
                                </RAIDDiskEntry>
                            </RAIDDiskTableConfig>
                        </xsl:when>
                        <xsl:when test="//DMS-1200-Device">
                            <RAIDType>RAID-10</RAIDType>
                            <RAIDDiskTableConfig Dirty="CLEAN">
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="1">
                                    <Port>1</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="2">
                                    <Port>2</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="3">
                                    <Port>3</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="4">
                                    <Port>4</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="5">
                                    <Port>5</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="6">
                                    <Port>6</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="7">
                                    <Port>7</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="8">
                                    <Port>8</Port>
                                </RAIDDiskEntry>
                            </RAIDDiskTableConfig>
                        </xsl:when>
                        <xsl:otherwise>
                            <RAIDType>RAID-10</RAIDType>
                            <RAIDDiskTableConfig Dirty="CLEAN">
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="1">
                                    <Port>1</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="2">
                                    <Port>2</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="3">
                                    <Port>3</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="4">
                                    <Port>4</Port>
                                </RAIDDiskEntry>
                            </RAIDDiskTableConfig>
                        </xsl:otherwise>
                    </xsl:choose>
                </RAIDArrayEntry>
                <MediaStore>
                    <Name>/data1</Name>
                    <RootDirPath>/data/DSS/data/portal/site
				</RootDirPath>
                </MediaStore>
            </HardDiskEntry>
            <xsl:choose>
                <xsl:when test="//DMS-1500-Device">
                    <HardDiskEntry Dirty="CLEAN" ListIndex="2">
                        <DeviceName>/dev/sdb1</DeviceName>
                        <MountPoint>/data2</MountPoint>
                        <RAIDArrayEntry>
                            <Controller>c0</Controller>
                            <Unit>u1</Unit>
                            <RAIDType>RAID-10</RAIDType>
                            <RAIDDiskTableConfig Dirty="CLEAN">
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="1">
                                    <Port>5</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="2">
                                    <Port>6</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="3">
                                    <Port>7</Port>
                                </RAIDDiskEntry>
                                <RAIDDiskEntry Dirty="CLEAN" ListIndex="4">
                                    <Port>8</Port>
                                </RAIDDiskEntry>
                            </RAIDDiskTableConfig>
                        </RAIDArrayEntry>
                        <MediaStore>
                            <Name>/data2</Name>
                            <RootDirPath>/data2/DSS/data/portal/site
				</RootDirPath>
                        </MediaStore>
                    </HardDiskEntry>
                    <NASConfig Dirty="CLEAN">
                        <NASTable>
                        </NASTable>
                    </NASConfig>
                </xsl:when>
            </xsl:choose>
        </HardDiskTableConfig>
    </xsl:template>

    <xsl:template mode="append" match="//DeviceConfig/ServiceElementsConfig/XMPPServiceConfig/IPVSDBService/Backup">
        <xsl:choose>
            <xsl:when test="EnableExport">
            </xsl:when>
            <xsl:otherwise>
                <EnableExport>0</EnableExport>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="ExportURL">
            </xsl:when>
            <xsl:otherwise>
                <ExportURL/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- copy device status only if serviceVer is of last migration -->
    <xsl:template match="//DeviceStatus">
        <xsl:variable name="rootname" select="name(/*)"/>
        <xsl:variable name="objectxml_file">
            <xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')"/>
        </xsl:variable>
        <xsl:variable name="objectXml_serviceVer" select="document($objectxml_file)/*/@serviceVer"/>
        <xsl:choose>
            <xsl:when test='"1.4"=$objectXml_serviceVer'>
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

