<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetPortResponse">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="GetPortResponse/Port ">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:copy-of select="./Groups" />
      <xsl:copy-of select="./FeatureList" />
    </record>
  </xsl:template> 

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:param name="NIDParam"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxEntityid">
    </xsl:attribute>    
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>   
    <xsl:attribute name="jsxVisible">0</xsl:attribute>    
    <xsl:attribute name="jsxdevpresence">
    </xsl:attribute>
    <xsl:attribute name="jsxdevicenid">
      <xsl:value-of select="$record_el/Asset/@deviceNID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxmaxInstanceCount">
      <xsl:value-of select="$record_el/MaxInstanceCount"/>
    </xsl:attribute>        
    <xsl:attribute name="jsxdevStreamType">
      <xsl:value-of select="$record_el/@streamType"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/@streamType='V2D'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MPEGTS'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='UDP'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='VNC'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='V2D MPEGTS RTP'">
        <xsl:attribute name="jsxdevtype">jsxaddin://user!ipvsCommon/images/icons/computer.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType=''">
        <xsl:attribute name="jsxdevtype"></xsl:attribute>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>