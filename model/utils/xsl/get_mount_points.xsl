<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 <xsl:output method="text" />
    <xsl:template match="/">
        <xsl:for-each select="//HardDiskEntry/MountPoint">
            <xsl:value-of select="."/><br/>
            <xsl:text>&#10;</xsl:text>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

