<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="groupID" />
  <xsl:param name="dateModified" />
  <xsl:param name="mediaStoreFile_Rev" />
  <xsl:param name="state">Busy</xsl:param>
  <xsl:template match="//MediaGroupID">
	<xsl:choose>
		<xsl:when test="string-length(normalize-space(.))>0">
			 <MediaGroupID><xsl:attribute name="ClipID"><xsl:value-of select="@ClipID" /></xsl:attribute><xsl:value-of select="$groupID" /></MediaGroupID>
		</xsl:when>
		<xsl:otherwise>
			<xsl:copy-of select="." />
		</xsl:otherwise>
	</xsl:choose>
  </xsl:template>
  <!--
  <xsl:template match="//DateModified">
    <DateModified><xsl:value-of select="$dateModified"/></DateModified>
  </xsl:template> 
  -->
	<xsl:template mode="append" match="/MediaStoreFile">
		<xsl:choose>
			<xsl:when test="BypassAutoStorageRules">					
			</xsl:when>
			<xsl:otherwise>
				<BypassAutoStorageRules>false</BypassAutoStorageRules>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
  <xsl:template match="/MediaStoreFile/@Rev">
    <xsl:attribute name="Rev">
      <xsl:value-of select="$mediaStoreFile_Rev" />
    </xsl:attribute>
  </xsl:template>
  <xsl:template match="//State">
    <State><xsl:value-of select="$state" /></State>
  </xsl:template>
  
  <!--  When HLSConfig is true or false, setting it to false 
  <xsl:template match="//RTPRecoderConfig/HLSConfig/Enable">
		<Enable><xsl:value-of select="string('false')"/></Enable>
  </xsl:template>-->
  
  <!--  When HLSConfig is absent, adding and disabling it -->
  <xsl:template mode="append" match="//RTPRecoderConfig[not(HLSConfig)]">
		<HLSConfig>
			<Enable>false</Enable>
		</HLSConfig>
  </xsl:template>
  
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
</xsl:stylesheet>