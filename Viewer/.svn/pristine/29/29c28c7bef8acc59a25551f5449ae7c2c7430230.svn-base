<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:gicache="http://xsd.tns.tibco.com/gi/cache">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>

  <xsl:template match="/">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="GetPort">
    <xsl:attribute name="startFrom">
      <xsl:value-of select="@from"/>
    </xsl:attribute>
    <xsl:attribute name="objectCount">
      <xsl:value-of select="@total"/>
    </xsl:attribute>
      <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetPort/Port">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
    </record>
  </xsl:template> 

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:param name="NIDParam"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>
    <xsl:attribute name="jsxdevstate">
      <xsl:value-of select="$record_el/@state"/>
    </xsl:attribute>
    <xsl:attribute name="jsxpresence">
      <xsl:value-of select="$record_el/@presence"/>
    </xsl:attribute>

    <xsl:choose>
      <xsl:when test="$record_el/@state='Ready'">
        <xsl:choose>
          <xsl:when test="$record_el/@thumbnailURL!=''">
            <xsl:attribute name="jsxthumbnailURL">
              <xsl:value-of select="$record_el/@thumbnailURL"/>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="jsxthumbnailURL">images/icons/srcthumb-online.png</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="$record_el/@state='Unmanaged'">
        <xsl:attribute name="jsxthumbnailURL">images/icons/srcthumb-unmanaged.png</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxthumbnailURL">images/icons/srcthumb-offline.png</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>