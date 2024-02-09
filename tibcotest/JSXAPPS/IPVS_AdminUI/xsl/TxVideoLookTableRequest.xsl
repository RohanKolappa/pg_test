<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <record>
            <xsl:attribute name="jsxid">
                  <xsl:value-of select="@VLTID"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupEntryDirtyBit">
                  <xsl:value-of select="@TxVideoLookupDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupEntryListIndex">
                <xsl:value-of select="@TxVideoLookupListIndex"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableID">
                  <xsl:value-of select="@VLTID"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableName">
                  <xsl:value-of select="@VLTName"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHres">
              <xsl:value-of select="@VLTHorzResolution"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVres">
                <xsl:value-of select="@VLTVertResolution"/>
            </xsl:attribute> 
            <xsl:attribute name="jsxLookupTableRefreshRate">
                <xsl:value-of select="@VLTRefreshRate"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTablePixelClockRate">
                <xsl:value-of select="@VLTPixelClock"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableStereo">
                <xsl:value-of select="@VLTStereo"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHFrontPorch">
                <xsl:value-of select="@VLTHorzFrontPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVFrontPorch">
                <xsl:value-of select="@VLTVertFrontPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHBackPorch">
                <xsl:value-of select="@VLTHorzBackPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVBackPorch">
                <xsl:value-of select="@VLTVertBackPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHSync">
                <xsl:value-of select="@VLTHorzSync"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVSync">
                <xsl:value-of select="@VLTVertSync"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVerticalMargin">
                <xsl:value-of select="@VLTVertMargin"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTablePhase">
                <xsl:value-of select="@VLTPhase"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHPos">
                <xsl:value-of select="@VLTHorzPosition"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVPos">
                <xsl:value-of select="@VLTVertPosition"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableCoarseNoise">
                <xsl:value-of select="@VLTCoarseNoise"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableFineNoise">
                <xsl:value-of select="@VLTFineNoise"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableGainRed">
                <xsl:value-of select="@VLTRedGain"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableGainGreen">
                <xsl:value-of select="@VLTGreenGain"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableGainBlue">
                <xsl:value-of select="@VLTBlueGain"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableOffsetRed">
                <xsl:value-of select="@VLTRedOffset"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableOffsetGreen">
                <xsl:value-of select="@VLTGreenOffset"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableOffsetBlue">
                <xsl:value-of select="@VLTBlueOffset"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableColorSpace">
                <xsl:value-of select="@VLTColorSpace"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableChargePumpCurr">
                <xsl:value-of select="@VLTChargePumpCur"/>
            </xsl:attribute>
        </record>
    </xsl:template>

 </xsl:stylesheet>
