<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <NATTable>
        <xsl:apply-templates/>
     </NATTable> 
    </xsl:template>

    <xsl:template match="record">
        <NATEntry>
          <Network><xsl:value-of select="@jsxNetwork"/></Network>
          <TranslatedNetwork><xsl:value-of select="@jsxTranslatedNetwork"/></TranslatedNetwork>
          <Netmask><xsl:value-of select="@jsxNetmask"/></Netmask>
        </NATEntry>
    </xsl:template>

 </xsl:stylesheet>
