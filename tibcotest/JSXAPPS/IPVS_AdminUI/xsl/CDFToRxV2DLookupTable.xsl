<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
       <V2DDecoderVideoLookupTable>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <xsl:apply-templates/>
       </V2DDecoderVideoLookupTable>
    </xsl:template>

    <xsl:template match="record">
        <V2DDecoderVideoLookupEntryConfig>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@jsxLookupEntryDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="ListIndex">
                <xsl:value-of select="@jsxLookupEntryListIndex"/>
            </xsl:attribute>
            <LUTID>
               <xsl:value-of select="@jsxLookupTableID"/>
            </LUTID> 
            <Name>
               <xsl:value-of select="@jsxLookupTableName"/>
            </Name>
            <Resolution>
                <Horizontal>
                   <xsl:value-of select="@jsxLookupTableHres"/>
                </Horizontal>
                <Vertical>
                   <xsl:value-of select="@jsxLookupTableVres"/>
                </Vertical>
            </Resolution> 
            <RefreshRate>
               <xsl:value-of select="@jsxLookupTableRefreshRate"/>
            </RefreshRate> 
            <PixelClockRate>
               <xsl:value-of select="@jsxLookupTablePixelClockRate"/>
            </PixelClockRate> 
            <Stereo>
               <xsl:value-of select="@jsxLookupTableStereo"/>
            </Stereo> 
            <FrontPorch>
                <Horizontal>
                   <xsl:value-of select="@jsxLookupTableHFrontPorch"/>
                </Horizontal>
                <Vertical>
                   <xsl:value-of select="@jsxLookupTableVFrontPorch"/>
                </Vertical>
            </FrontPorch> 
            <BackPorch>
                <Horizontal>
                   <xsl:value-of select="@jsxLookupTableHBackPorch"/>
                </Horizontal>
                <Vertical>
                   <xsl:value-of select="@jsxLookupTableVBackPorch"/>
                </Vertical>
            </BackPorch> 
            <Sync>
                <Horizontal>
                   <xsl:value-of select="@jsxLookupTableHSync"/>
                </Horizontal>
                <Vertical>
                   <xsl:value-of select="@jsxLookupTableVSync"/>
                </Vertical>
            </Sync> 
        </V2DDecoderVideoLookupEntryConfig>
    </xsl:template>

 </xsl:stylesheet>
