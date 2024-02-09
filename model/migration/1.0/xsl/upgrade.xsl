<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
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
        <NATTraversalTableConfig Dirty="CLEAN"/>
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

