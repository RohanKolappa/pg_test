<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>

  <xsl:param name="CanvasEditTypeArgument"></xsl:param>
  
  <xsl:template match="/">
    <data jsxid="jsxroot">
      <xsl:apply-templates />
    </data>
  </xsl:template>

  <xsl:template match="data/record/Canvas/Window">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
        <xsl:with-param name="CanvasEditType" select="$CanvasEditTypeArgument"/>
      </xsl:call-template>
    </record>
  </xsl:template>

  <xsl:template name="record_attributes">
    <xsl:param name="CanvasEditType"/>
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@id"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@id"/>
    </xsl:attribute>
    <xsl:attribute name="enable">
      <xsl:value-of select="$record_el/@enable"/>
    </xsl:attribute>
    <xsl:attribute name="content">
      <xsl:value-of select="$record_el/@content"/>
    </xsl:attribute>
    <xsl:attribute name="hpos">
      <xsl:value-of select="Properties/@hpos"/>
    </xsl:attribute>
    <xsl:attribute name="vpos">
      <xsl:value-of select="Properties/@vpos"/>
    </xsl:attribute>
    <xsl:attribute name="hres">
      <xsl:value-of select="Properties/@hres"/>
    </xsl:attribute>
    <xsl:attribute name="vres">
      <xsl:value-of select="Properties/@vres"/>
    </xsl:attribute>
    <xsl:attribute name="aspect">
      <xsl:value-of select="Properties/@aspect"/>
    </xsl:attribute>
    <xsl:attribute name="transparency">
      <xsl:value-of select="Properties/@transparency"/>
    </xsl:attribute>
    <xsl:attribute name="mute">
      <xsl:value-of select="Properties/@mute"/>
    </xsl:attribute>
     <xsl:attribute name="visibility">
      <xsl:value-of select="Properties/@visibility"/>
    </xsl:attribute>
    <xsl:attribute name="scale">
      <xsl:value-of select="Properties/@scale"/>
    </xsl:attribute>
    <xsl:attribute name="maximize">
      <xsl:value-of select="Properties/@maximize"/>
    </xsl:attribute>
    <xsl:attribute name="zIndex">
      <xsl:value-of select="Properties/@zIndex"/>
    </xsl:attribute>
    <xsl:attribute name="border">
      <xsl:value-of select="Properties/@border"/>
    </xsl:attribute>
    <xsl:attribute name="layoutId">
      <xsl:value-of select="Properties/@layoutId"/>
    </xsl:attribute>
    <xsl:attribute name="layoutPos">
      <xsl:value-of select="Properties/@layoutPos"/>
    </xsl:attribute>
    <xsl:attribute name="volume">
      <xsl:value-of select="Properties/@volume"/>
    </xsl:attribute>
    <xsl:attribute name="remoteKbmCtrl">
      <xsl:value-of select="Properties/@remoteKbmCtrl"/>
    </xsl:attribute>
    <xsl:attribute name="scrollHPos">
      <xsl:value-of select="Properties/@scrollHPos"/>
    </xsl:attribute>
    <xsl:attribute name="scrollVPos">
      <xsl:value-of select="Properties/@scrollVPos"/>
    </xsl:attribute>
    <xsl:attribute name="srcKbmCtrl">
      <xsl:value-of select="ConnectionContent/@srcKbmCtrl"/>
    </xsl:attribute>
    <xsl:attribute name="jsxCanvasEditType">
      <xsl:value-of select="$CanvasEditType"/>
    </xsl:attribute>
    
  </xsl:template>
</xsl:stylesheet>