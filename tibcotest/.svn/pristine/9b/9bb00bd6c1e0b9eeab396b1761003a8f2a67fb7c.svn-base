<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>

  <xsl:template match="/">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="//PROJECT/MEDIALIST">
    <xsl:for-each select="MEDIA">
      <record>
        <xsl:call-template name="record_attributes1">
          <xsl:with-param name="record_el" select="."/>
        </xsl:call-template>
        <xsl:apply-templates select="CLIP"></xsl:apply-templates>
      </record>
    </xsl:for-each>
    <xsl:for-each select="CLIP">
      <xsl:choose>
      <xsl:when test="@isActive!='false'">
        <record>
          <xsl:call-template name="record_attributes">
            <xsl:with-param name="record_el" select="."/>
          </xsl:call-template>
        </record>
      </xsl:when>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="MEDIA/CLIP">
    <xsl:choose>
      <xsl:when test="@isActive!='false'">
        <record>
          <xsl:call-template name="record_attributes">
            <xsl:with-param name="record_el" select="."/>
          </xsl:call-template>
        </record>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="record_attributes1">
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@ID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxfileType">MEDIAGROUP</xsl:attribute>
    <xsl:attribute name="jsxtextmediaName">
      <xsl:value-of select="$record_el/TITLE"/>
    </xsl:attribute>
    <xsl:attribute name="jsxSdpInfo">
      <xsl:value-of select="$record_el/SDPINFO"/>
    </xsl:attribute>
    <xsl:attribute name="jsxStreamType">
      <xsl:value-of select="$record_el/TYPE"/>
    </xsl:attribute>    
    <xsl:choose>
      <xsl:when test="number($record_el/SIZE)">
        <xsl:attribute name="jsxtextmediaSize">
          <xsl:value-of select='round($record_el/SIZE div 1048576)'/>
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxtextmediaSize">0</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:variable name="ms" select="$record_el/PLAYLENGTH" />
    <xsl:variable name="sec" select="floor($ms div 1000)" />
    <xsl:variable name="min" select="floor($sec div 60)"/>
    <xsl:variable name="hr" select="floor($min div 60)"/>
    <xsl:variable name="t41">
      <xsl:choose>
        <xsl:when test="($ms mod 1000)> 99">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t42">
      <xsl:choose>
        <xsl:when test="($ms mod 1000)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t4" select="concat($t41,concat($t42,($ms mod 1000)))"/>
    <xsl:variable name="t31">
      <xsl:choose>
        <xsl:when test="($sec mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t3" select="concat($t31,($sec mod 60),':')"/>
    <xsl:variable name="t21">
      <xsl:choose>
        <xsl:when test="($min mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t2" select="concat($t21,($min mod 60),':')"/>
    <xsl:variable name="t11">
      <xsl:choose>
        <xsl:when test="($hr mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t1" select="concat($t11,($hr mod 60),':')"/>
    <xsl:attribute name="jsxtextmediaLength">
      <xsl:value-of select="concat(concat($t1,$t2),concat($t3,$t4))"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextmediaCreated">
      <xsl:value-of select="$record_el/DATETIME"/>
    </xsl:attribute>
  </xsl:template>

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@ID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxfileType">
      <xsl:value-of select="$record_el/TYPE"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextmediaName">
      <xsl:value-of select="$record_el/TITLE"/>
    </xsl:attribute>
    <xsl:attribute name="jsxSdpInfo">
      <xsl:value-of select="$record_el/SDPINFO"/>
    </xsl:attribute>
    <xsl:attribute name="jsxStreamType">
      <xsl:value-of select="$record_el/TYPE"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="number($record_el/SIZE)">
        <xsl:attribute name="jsxtextmediaSize">
          <xsl:value-of select='round($record_el/SIZE div 1048576)'/>
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxtextmediaSize">0</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:variable name="ms" select="$record_el/PLAYLENGTH" />
    <xsl:variable name="sec" select="floor($ms div 1000)" />
    <xsl:variable name="min" select="floor($sec div 60)"/>
    <xsl:variable name="hr" select="floor($min div 60)"/>
    <xsl:variable name="t41">
      <xsl:choose>
        <xsl:when test="($ms mod 1000)> 99">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t42">
      <xsl:choose>
        <xsl:when test="($ms mod 1000)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t4" select="concat($t41,concat($t42,($ms mod 1000)))"/>
    <xsl:variable name="t31">
      <xsl:choose>
        <xsl:when test="($sec mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t3" select="concat($t31,($sec mod 60),':')"/>
    <xsl:variable name="t21">
      <xsl:choose>
        <xsl:when test="($min mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t2" select="concat($t21,($min mod 60),':')"/>
    <xsl:variable name="t11">
      <xsl:choose>
        <xsl:when test="($hr mod 60)> 9">
          <xsl:value-of select="''"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'0'"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="t1" select="concat($t11,($hr mod 60),':')"/>
    <xsl:attribute name="jsxtextmediaLength">
      <xsl:value-of select="concat(concat($t1,$t2),concat($t3,$t4))"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextmediaCreated">
      <xsl:value-of select="$record_el/DATETIME"/>
    </xsl:attribute>
    <xsl:attribute name="jsxclipID">
      <xsl:value-of select="$record_el/FILENAME"/>
    </xsl:attribute>

  </xsl:template>
</xsl:stylesheet>
