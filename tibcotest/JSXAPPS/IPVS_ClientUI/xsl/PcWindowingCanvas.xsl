<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
  <xsl:template match="/">
      <xsl:apply-templates />
  </xsl:template>

  <!--<xsl:template match="GetPortDisplayStatus">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>-->


  <xsl:template match="GetPortDisplayStatus/PortDisplayStatus">
    <data jsxid="jsxroot">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:copy-of select="Display" />
      <xsl:copy-of select="Canvas" />
    </record>
    </data>
  </xsl:template>

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>
  </xsl:template>
  
</xsl:stylesheet>