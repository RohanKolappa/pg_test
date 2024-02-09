<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
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

    <!-- if exists serviceVer -->
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer">2.2</xsl:attribute>
    </xsl:template>
    <xsl:template match="//DeviceElementID[.='1']">
		<DeviceElementID>2</DeviceElementID>
    </xsl:template>
    <xsl:template match="//DeviceElementID[.='0']">
		<DeviceElementID>1</DeviceElementID>
    </xsl:template>
    <xsl:template match="//DeviceElementID[string-length(.)=0]">
		<DeviceElementID>1</DeviceElementID>
    </xsl:template>
</xsl:stylesheet>
