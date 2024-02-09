<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <!--<xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>-->

  <xsl:template match="MEDIALIST">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="MEDIALIST/MEDIA">
    <record>
      <xsl:call-template name="translate_group_record">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:apply-templates select="CLIP"></xsl:apply-templates>
    </record>
  </xsl:template>

 <xsl:template match="MEDIALIST/CLIP">
    <record>
      <xsl:attribute name="jsxid">
        <xsl:value-of select="@ID"/>
      </xsl:attribute>
      <xsl:attribute name="jsxActive">
        <xsl:value-of select="@isActive"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="@isActive='false'">
           <xsl:attribute name="jsxstyle">display:none</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
           <xsl:attribute name="jsxstyle">display:block</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:attribute name="isRoot"><xsl:value-of select="@isRoot"/></xsl:attribute>
      <xsl:attribute name="jsxFileNID"><xsl:value-of select="FILENAME"/></xsl:attribute>
      <xsl:attribute name="jsxtextmediaName">
        <xsl:value-of select="TITLE"/>
      </xsl:attribute>
      <xsl:attribute name="jsxCreatedDate">
        <xsl:value-of select="DATETIME"/>
      </xsl:attribute>
      <xsl:attribute name="jsxLength">
        <xsl:value-of select="LENGTH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxPlayLength">
        <xsl:value-of select="PLAYLENGTH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxSize">
        <xsl:value-of select="SIZE"/>
      </xsl:attribute>
      <xsl:attribute name="jsxGrpNID"></xsl:attribute>
      <xsl:attribute name="jsxBasePath">
        <xsl:value-of select="BASEPATH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxSDPInfo">
        <xsl:value-of select="SDPINFO"/>
      </xsl:attribute>
      <xsl:attribute name="jsxtextprocessingstatus">Queued</xsl:attribute>
    </record>
  </xsl:template>

  <xsl:template match="MEDIALIST/MEDIA/CLIP">
    <record>
      <xsl:attribute name="jsxid">
        <xsl:value-of select="@ID"/>
      </xsl:attribute>
      <xsl:attribute name="jsxActive">
        <xsl:value-of select="@isActive"/>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="@isActive='false'">
           <xsl:attribute name="jsxstyle">display:none</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
           <xsl:attribute name="jsxstyle">display:block</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:attribute name="isRoot"><xsl:value-of select="@isRoot"/></xsl:attribute>
      <xsl:attribute name="jsxFileNID"><xsl:value-of select="FILENAME"/></xsl:attribute>
      <xsl:attribute name="jsxtextmediaName">
        <xsl:value-of select="TITLE"/>
      </xsl:attribute>
      <xsl:attribute name="jsxCreatedDate">
        <xsl:value-of select="DATETIME"/>
      </xsl:attribute>
      <xsl:attribute name="jsxLength">
        <xsl:value-of select="LENGTH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxPlayLength">
        <xsl:value-of select="PLAYLENGTH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxSize">
        <xsl:value-of select="SIZE"/>
      </xsl:attribute>
      <xsl:attribute name="jsxGrpNID"></xsl:attribute>
      <xsl:attribute name="jsxBasePath">
        <xsl:value-of select="BASEPATH"/>
      </xsl:attribute>
      <xsl:attribute name="jsxSDPInfo">
        <xsl:value-of select="SDPINFO"/>
      </xsl:attribute>
      <xsl:attribute name="jsxtextprocessingstatus">Queued</xsl:attribute>
    </record>
  </xsl:template>

  <xsl:template name="translate_group_record">
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@ID"/></xsl:attribute>
      <xsl:attribute name="isRoot"><xsl:value-of select="$record_el/@isRoot"/></xsl:attribute>
      <xsl:attribute name="jsxFileNID"><xsl:value-of select="$record_el/FILENAME"/></xsl:attribute>
    <xsl:attribute name="jsxtextmediaName">
      <xsl:value-of select="$record_el/TITLE"/>
    </xsl:attribute>
    <xsl:attribute name="jsxCreatedDate">
      <xsl:value-of select="$record_el/DATETIME"/>
    </xsl:attribute>
    <xsl:attribute name="jsxLength">
      <xsl:value-of select="LENGTH"/>
    </xsl:attribute>
    <xsl:attribute name="jsxPlayLength">
      <xsl:value-of select="PLAYLENGTH"/>
    </xsl:attribute>
    <xsl:attribute name="jsxSize">
      <xsl:value-of select="$record_el/SIZE"/>
    </xsl:attribute>
    <xsl:attribute name="jsxGrpNID"></xsl:attribute>
    <xsl:attribute name="jsxBasePath">
      <xsl:value-of select="$record_el/BASEPATH"/>
    </xsl:attribute>
    <xsl:attribute name="jsxSDPInfo">
      <xsl:value-of select="$record_el/SDPINFO"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextprocessingstatus">Queued</xsl:attribute>
    <xsl:attribute name="jsxopen">1</xsl:attribute>
  </xsl:template>
</xsl:stylesheet>