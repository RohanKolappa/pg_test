<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes"  />
  <xsl:param name="MS_JID"></xsl:param>
  <xsl:param name="serviceVer">2.24</xsl:param>
  <!-- Here's the boilerplate code -->
  <!-- By default, copy all nodes unchanged -->
  <xsl:template match="@* | node()">
    <xsl:apply-templates mode="before" select="." />
    <xsl:copy>
      <xsl:apply-templates select="@*" />
      <xsl:apply-templates mode="add-atts" select="." />
      <xsl:apply-templates mode="insert" select="." />
      <xsl:apply-templates />
      <xsl:apply-templates mode="append" select="." />
    </xsl:copy>
    <xsl:apply-templates mode="after" select="." />
  </xsl:template>
  <!-- By default, don't add anything -->
  <xsl:template mode="add-atts" match="*" />
  <xsl:template mode="insert" match="*" />
  <xsl:template mode="append" match="*" />
  <xsl:template mode="before" match="@* | node()" />
  <xsl:template mode="after" match="@* | node()" />

  

  <!-- copy device status only if serviceVer is of last migration -->
  <xsl:template match="//Request">
    <Response>
      <xsl:attribute name="serviceVer"> <xsl:value-of select="$serviceVer" /> </xsl:attribute>  
      <xsl:apply-templates />
    </Response>
  </xsl:template>
  

  <xsl:template mode="append" match="//header">
      <state>8</state>
  </xsl:template>

  <xsl:template match="//AgentMessage/@from">
    <xsl:attribute name="from"><xsl:value-of select="//AgentMessage/@to" /></xsl:attribute>
  </xsl:template>

  <xsl:template match="//AgentMessage/@type">
    <xsl:attribute name="type">Response</xsl:attribute>
  </xsl:template>

  <xsl:template match="//AgentMessage/@to">
    <xsl:attribute name="to"><xsl:value-of select="//AgentMessage/@from" /></xsl:attribute>
  </xsl:template>

 <xsl:template match="//AgentMessage/@logLevel">
    <xsl:attribute name="logLevel">5</xsl:attribute>
  </xsl:template>

  <xsl:template match="//header/userJID">
    <userJID>
      <xsl:value-of select="$MS_JID" />
    </userJID>
  </xsl:template>

  <xsl:template match="//Request/data//ImportFilePackRequestData">
    <ImportFilePackResponseData>
      <xsl:apply-templates />
    </ImportFilePackResponseData>
  </xsl:template>

  <xsl:template match="//Request/data//ImportCleanupMediaFileRequestData">
    <ImportCleanupMediaFileResponseData>
      <xsl:apply-templates />
    </ImportCleanupMediaFileResponseData>
  </xsl:template>


</xsl:stylesheet>
