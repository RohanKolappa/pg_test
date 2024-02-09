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
        <xsl:attribute name="RxVideoLookupDirtyBit">
            <xsl:value-of select="$record_el/@jsxLookupEntryDirtyBit"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupListIndex">
            <xsl:value-of select="$record_el/@jsxLookupEntryListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupID">
            <xsl:value-of select="$record_el/@jsxLookupTableID"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupName">
            <xsl:value-of select="$record_el/@jsxLookupTableName"/>
        </xsl:attribute> 
        <xsl:attribute name="RxVideoLookupHres">
            <xsl:value-of select="$record_el/@jsxLookupTableHres"/>
        </xsl:attribute> 
        <xsl:attribute name="RxVideoLookupVres">
            <xsl:value-of select="$record_el/@jsxLookupTableVres"/>
        </xsl:attribute> 
        <xsl:attribute name="RxVideoLookupRefreshRate">
            <xsl:value-of select="$record_el/@jsxLookupTableRefreshRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupPixelClockRate">
            <xsl:value-of select="$record_el/@jsxLookupTablePixelClockRate"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupStereo">
            <xsl:value-of select="$record_el/@jsxLookupTableStereo"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupHfrontPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableHFrontPorch"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupVfrontPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableVFrontPorch"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupHbackPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableHBackPorch"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupVbackPorch">
            <xsl:value-of select="$record_el/@jsxLookupTableVBackPorch"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupHsync">
            <xsl:value-of select="$record_el/@jsxLookupTableHSync"/>
        </xsl:attribute>
        <xsl:attribute name="RxVideoLookupVsync">
            <xsl:value-of select="$record_el/@jsxLookupTableVSync"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>