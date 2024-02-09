<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <V2DCodecVideoLookupTable>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <xsl:apply-templates/>
      </V2DCodecVideoLookupTable>
    </xsl:template>

    <xsl:template match="record">
        <V2DCodecVideoLookupEntryConfig>
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
            <Phase>
               <xsl:value-of select="@jsxLookupTablePhase"/>
            </Phase>
            <VerticalMargin>
               <xsl:value-of select="@jsxLookupTableVerticalMargin"/>
            </VerticalMargin> 
            <Position>
                <Horizontal>
                   <xsl:value-of select="@jsxLookupTableHPos"/>
                </Horizontal>
                <Vertical>
                   <xsl:value-of select="@jsxLookupTableVPos"/>
                </Vertical>
            </Position>
            <RefreshRate>
               <xsl:value-of select="@jsxLookupTableRefreshRate"/>
            </RefreshRate> 
            <PixelClockRate>
               <xsl:value-of select="@jsxLookupTablePixelClockRate"/>
            </PixelClockRate> 
            <NoiseElimination>
                <Coarse>
                   <xsl:value-of select="@jsxLookupTableCoarseNoise"/>
                </Coarse>
                <Fine>
                   <xsl:value-of select="@jsxLookupTableFineNoise"/>
                </Fine>
            </NoiseElimination>
            <Stereo>
               <xsl:value-of select="@jsxLookupTableStereo"/>
            </Stereo> 
            <ColorSpace>
               <xsl:value-of select="@jsxLookupTableColorSpace"/>
            </ColorSpace> 
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
            <ChargePumpCurr>
               <xsl:value-of select="@jsxLookupTableChargePumpCurr"/>
            </ChargePumpCurr> 
            <Gain>
                <Red>
                   <xsl:value-of select="@jsxLookupTableGainRed"/>
                </Red>
                <Green>
                   <xsl:value-of select="@jsxLookupTableGainGreen"/>
                </Green>
                <Blue>
                   <xsl:value-of select="@jsxLookupTableGainBlue"/>
                </Blue>
            </Gain>
            <Offset>
                <Red>
                   <xsl:value-of select="@jsxLookupTableOffsetRed"/>
                </Red>
                <Green>
                   <xsl:value-of select="@jsxLookupTableOffsetGreen"/>
                </Green>
                <Blue>
                   <xsl:value-of select="@jsxLookupTableOffsetBlue"/>
                </Blue>
            </Offset> 
        </V2DCodecVideoLookupEntryConfig>
    </xsl:template>

 </xsl:stylesheet>
