<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <record>
            <xsl:attribute name="jsxid">
                  <xsl:value-of select="@RxVideoLookupID"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupEntryDirtyBit">
                  <xsl:value-of select="@RxVideoLookupDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupEntryListIndex">
                <xsl:value-of select="@RxVideoLookupListIndex"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableID">
                  <xsl:value-of select="@RxVideoLookupID"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableName">
                  <xsl:value-of select="@RxVideoLookupName"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHres">
              <xsl:value-of select="@RxVideoLookupHres"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVres">
                <xsl:value-of select="@RxVideoLookupVres"/>
            </xsl:attribute> 
            <xsl:attribute name="jsxLookupTableRefreshRate">
                <xsl:value-of select="@RxVideoLookupRefreshRate"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTablePixelClockRate">
                <xsl:value-of select="@RxVideoLookupPixelClockRate"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableStereo">
                <xsl:value-of select="@RxVideoLookupStereo"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHFrontPorch">
                <xsl:value-of select="@RxVideoLookupHfrontPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVFrontPorch">
                <xsl:value-of select="@RxVideoLookupVfrontPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHBackPorch">
                <xsl:value-of select="@RxVideoLookupHbackPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVBackPorch">
                <xsl:value-of select="@RxVideoLookupVbackPorch"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableHSync">
                <xsl:value-of select="@RxVideoLookupHsync"/>
            </xsl:attribute>
            <xsl:attribute name="jsxLookupTableVSync">
                <xsl:value-of select="@RxVideoLookupVsync"/>
            </xsl:attribute>
        </record>
    </xsl:template>

 </xsl:stylesheet>
