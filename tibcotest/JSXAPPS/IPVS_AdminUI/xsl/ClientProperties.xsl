<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="data">
    <data jsxid="jsxroot">
      <record jsxid="OwnerUserJID" jsxtext="OwnerUserJID" jsxName="ownerUserJID"/>
      <record jsxid="Presence" jsxtext="Presence" jsxName="presence"/>
      <record jsxid="State" jsxtext="State" jsxName="state"/>
      <record jsxid="Title" jsxtext="Title" jsxName="title"/>
      <record jsxid="Description" jsxtext="Description" jsxName="description"/>
      <record jsxid="Type" jsxtext="Type" jsxName="type"/>
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="data/record">
    <record>
      <xsl:attribute name="jsxid">
        <xsl:value-of select="@jsxid"/>
      </xsl:attribute>
      <xsl:attribute name="jsxtext">
        <xsl:value-of select="@jsxtext"/>
      </xsl:attribute>
      <xsl:attribute name="jsxName">
        <xsl:value-of select="@jsxtext"/>
      </xsl:attribute>
    </record>
  </xsl:template>
</xsl:stylesheet>