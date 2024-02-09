<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="V2DDecoderVideoLookupTable">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="V2DDecoderVideoLookupTable/V2DDecoderVideoLookupEntryConfig">
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
        <xsl:attribute name="jsxLookupTableRefreshRate">
            <xsl:value-of select="$record_el/RefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTablePixelClockRate">
            <xsl:value-of select="$record_el/PixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLookupTableStereo">
            <xsl:value-of select="$record_el/Stereo"/>
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
     </xsl:template>
</xsl:stylesheet>