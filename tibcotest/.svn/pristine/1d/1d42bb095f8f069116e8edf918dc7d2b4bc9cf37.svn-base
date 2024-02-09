<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <NASConfig>
        <xsl:attribute name="Dirty"></xsl:attribute>
        <NASTable>
            <xsl:apply-templates/>
        </NASTable>
      </NASConfig>
    </xsl:template>

    <xsl:template match="record">
            <NASEntry>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@jsxDirtyBit"/>
            </xsl:attribute>
            <xsl:attribute name="ListIndex">
                <xsl:value-of select="@jsxListIndex"/>
            </xsl:attribute>
            <Server>
               <xsl:value-of select="@jsxServer"/>
            </Server> 
            <Enable>
                <xsl:choose>
                  <xsl:when test="@jsxEnable='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
            </Enable>
            <ReadOnly>
                <xsl:choose>
                  <xsl:when test="@jsxReadOnly='1'">true</xsl:when>
                  <xsl:otherwise>false</xsl:otherwise>
                </xsl:choose>
            </ReadOnly>
            <RemoteDirectory>
               <xsl:value-of select="@jsxRemoteDir"/>
            </RemoteDirectory>
            <MediaStore>
            <Name>
               <xsl:value-of select="@jsxMediaStoreName"/>
            </Name>
            <RootDirPath>
               <xsl:value-of select="@jsxMediaStoreRootDirPath"/>
            </RootDirPath>
            </MediaStore>
            </NASEntry>
    </xsl:template>
 </xsl:stylesheet>
