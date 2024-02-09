<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="V2DCodecVideoLookupTable">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="V2DCodecVideoLookupTable/V2DCodecVideoLookupEntryConfig">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupEntryDirtyBit">
            <xsl:value-of select="$record_el/@Dirty"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupEntryListIndex">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableID">
            <xsl:value-of select="$record_el/LUTID"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableName">
            <xsl:value-of select="$record_el/Name"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableHres">
            <xsl:value-of select="$record_el/Resolution/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVres">
            <xsl:value-of select="$record_el/Resolution/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTablePhase">
            <xsl:value-of select="$record_el/Phase"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVerticalMargin">
            <xsl:value-of select="$record_el/VerticalMargin"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableHPos">
            <xsl:value-of select="$record_el/Position/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVPos">
            <xsl:value-of select="$record_el/Position/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableRefreshRate">
            <xsl:value-of select="$record_el/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTablePixelClockRate">
            <xsl:value-of select="$record_el/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableCoarseNoise">
            <xsl:value-of select="$record_el/NoiseElimination/Coarse"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableFineNoise">
            <xsl:value-of select="$record_el/NoiseElimination/Fine"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableStereo">
            <xsl:value-of select="$record_el/Stereo"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableColorSpace">
            <xsl:value-of select="$record_el/ColorSpace"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableHFrontPorch">
            <xsl:value-of select="$record_el/FrontPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVFrontPorch">
            <xsl:value-of select="$record_el/FrontPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableHBackPorch">
            <xsl:value-of select="$record_el/BackPorch/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVBackPorch">
            <xsl:value-of select="$record_el/BackPorch/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableHSync">
            <xsl:value-of select="$record_el/Sync/Horizontal"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableVSync">
            <xsl:value-of select="$record_el/Sync/Vertical"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableChargePumpCurr">
            <xsl:value-of select="$record_el/ChargePumpCurr"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableGainRed">
            <xsl:value-of select="$record_el/Gain/Red"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableGainGreen">
            <xsl:value-of select="$record_el/Gain/Green"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableGainBlue">
            <xsl:value-of select="$record_el/Gain/Blue"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableOffsetRed">
            <xsl:value-of select="$record_el/Offset/Red"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableOffsetGreen">
            <xsl:value-of select="$record_el/Offset/Green"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableOffsetBlue">
            <xsl:value-of select="$record_el/Offset/Blue"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>