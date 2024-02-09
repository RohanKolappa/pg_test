<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="V2DCodecVideoLookupTable/record">
        <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
      <xsl:attribute name="jsxid">
        <xsl:value-of select="$record_el/@jsxid"/>
      </xsl:attribute>
      <xsl:attribute name="TxVideoLookupDirtyBit">
            <xsl:value-of select="$record_el/@jsxLookupEntryDirtyBit"/>
        </xsl:attribute>
        <xsl:attribute name="TxVideoLookupListIndex">
            <xsl:value-of select="$record_el/@jsxLookupEntryListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="VLTID">
            <xsl:value-of select="$record_el/@jsxLookupTableID"/>
        </xsl:attribute>
        <xsl:attribute name="VLTName">
            <xsl:value-of select="$record_el/@jsxLookupTableName"/>
        </xsl:attribute> 
        <xsl:attribute name="VLTHorzResolution">
            <xsl:value-of select="$record_el/@jsxLookupTableHres"/>
        </xsl:attribute> 
        <xsl:attribute name="VLTVertResolution">
            <xsl:value-of select="$record_el/@jsxLookupTableVres"/>
        </xsl:attribute> 
        <xsl:attribute name="VLTRefreshRate">
            <xsl:value-of select="$record_el/@jsxLookupTableRefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="VLTPixelClock">
            <xsl:value-of select="$record_el/@jsxLookupTablePixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="VLTStereo">
            <xsl:value-of select="$record_el/@jsxLookupTableStereo"/>
        </xsl:attribute>
        <xsl:attribute name="VLTHorzFrontPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableHFrontPorch"/>
        </xsl:attribute>
        <xsl:attribute name="VLTVertFrontPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableVFrontPorch"/>
        </xsl:attribute>
        <xsl:attribute name="VLTHorzBackPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableHBackPorch"/>
        </xsl:attribute>
        <xsl:attribute name="VLTVertBackPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableVBackPorch"/>
        </xsl:attribute>
        <xsl:attribute name="VLTHorzSync">
            <xsl:value-of select="$record_el/@jsxLookupTableHSync"/>
        </xsl:attribute>
        <xsl:attribute name="VLTVertSync">
            <xsl:value-of select="$record_el/@jsxLookupTableVSync"/>
        </xsl:attribute>
        <xsl:attribute name="VLTVertMargin">
            <xsl:value-of select="$record_el/@jsxLookupTableVerticalMargin"/>
        </xsl:attribute>
        <xsl:attribute name="VLTPhase">
            <xsl:value-of select="$record_el/@jsxLookupTablePhase"/>
        </xsl:attribute>
        <xsl:attribute name="VLTHorzPosition">
            <xsl:value-of select="$record_el/@jsxLookupTableHPos"/>
        </xsl:attribute>
        <xsl:attribute name="VLTVertPosition">
            <xsl:value-of select="$record_el/@jsxLookupTableVPos"/>
        </xsl:attribute>
        <xsl:attribute name="VLTCoarseNoise">
            <xsl:value-of select="$record_el/@jsxLookupTableCoarseNoise"/>
        </xsl:attribute>
        <xsl:attribute name="VLTFineNoise">
            <xsl:value-of select="$record_el/@jsxLookupTableFineNoise"/>
        </xsl:attribute>
        <xsl:attribute name="VLTRedGain">
            <xsl:value-of select="$record_el/@jsxLookupTableGainRed"/>
        </xsl:attribute>
        <xsl:attribute name="VLTGreenGain">
            <xsl:value-of select="$record_el/@jsxLookupTableGainGreen"/>
        </xsl:attribute>
        <xsl:attribute name="VLTBlueGain">
            <xsl:value-of select="$record_el/@jsxLookupTableGainBlue"/>
        </xsl:attribute>
        <xsl:attribute name="VLTRedOffset">
            <xsl:value-of select="$record_el/@jsxLookupTableOffsetRed"/>
        </xsl:attribute>
        <xsl:attribute name="VLTGreenOffset">
            <xsl:value-of select="$record_el/@jsxLookupTableOffsetGreen"/>
        </xsl:attribute>
        <xsl:attribute name="VLTBlueOffset">
            <xsl:value-of select="$record_el/@jsxLookupTableOffsetBlue"/>
        </xsl:attribute>
        <xsl:attribute name="VLTColorSpace">
            <xsl:value-of select="$record_el/@jsxLookupTableColorSpace"/>
        </xsl:attribute>
        <xsl:attribute name="VLTChargePumpCur">
            <xsl:value-of select="$record_el/@jsxLookupTableChargePumpCurr"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>