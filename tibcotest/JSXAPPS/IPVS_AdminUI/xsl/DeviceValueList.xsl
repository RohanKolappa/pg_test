<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="device">
    <data jsxid="jsxroot">
      <xsl:copy-of select="./record" />
    </data>
  </xsl:template>
</xsl:stylesheet>