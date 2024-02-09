<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <Groups>
        <xsl:apply-templates/>
     </Groups> 
    </xsl:template>

    <xsl:template match="record">
        <Tag>
          <xsl:attribute name="Name">
              <xsl:value-of select="@jsxTagName"/>
          </xsl:attribute>
          <xsl:attribute name="Value">
              <xsl:value-of select="@jsxTagValue"/>
          </xsl:attribute>
        </Tag>
    </xsl:template>

 </xsl:stylesheet>
