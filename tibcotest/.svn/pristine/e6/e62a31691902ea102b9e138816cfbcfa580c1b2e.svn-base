<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <DeviceConfig>
              <StorageElementsConfig>
                  <NASConfig>
                      <xsl:attribute name="Dirty">
                          <xsl:value-of select="@DirtyBitblkNASTable"/>
                      </xsl:attribute>
                  </NASConfig>
              </StorageElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
