<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetReport">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="GetReport/Report">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
    </record>
  </xsl:template>


  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@name"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/@type ='Policy'">
            <xsl:attribute name="jsxtype">Database Configuration</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
            <xsl:attribute name="jsxtype">Audit</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="jsxformat">
      <xsl:value-of select="$record_el/@format"/>
    </xsl:attribute>
    <xsl:attribute name="jsxdescription">
      <xsl:value-of select="$record_el/@description"/>
    </xsl:attribute>
    <xsl:attribute name="jsxenableExport">
      <xsl:value-of select="$record_el/@enableExport"/>
    </xsl:attribute>
    <xsl:attribute name="jsxexportSchedule">
      <xsl:value-of select="$record_el/@exportSchedule"/>
    </xsl:attribute>
    <xsl:attribute name="jsxexportURL">
      <xsl:value-of select="$record_el/@exportURL"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/@type='Audit'">
        <xsl:attribute name="jsxproperties">
          <xsl:value-of select="$record_el/Audit/@properties"/>
        </xsl:attribute>
        <xsl:attribute name="jsxpermissions">
        </xsl:attribute>
        <xsl:attribute name="jsxtarget">
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxproperties">
          <xsl:value-of select="$record_el/Policy/@properties"/>
        </xsl:attribute>
        <xsl:attribute name="jsxpermissions">
          <xsl:value-of select="$record_el/Policy/@permissions"/>
        </xsl:attribute>
        <xsl:attribute name="jsxtarget">
          <xsl:value-of select="$record_el/Policy/@whereTarget"/>
        </xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="jsxlastrundate">
      <xsl:value-of select="$record_el/Status/LastRunStatus/@date"/>
    </xsl:attribute>
    <xsl:attribute name="jsxlastruntime">
      <xsl:value-of select="$record_el/Status/LastRunStatus/@time"/>
    </xsl:attribute>
    <xsl:attribute name="jsxlastrunstate">
      <xsl:value-of select="$record_el/Status/LastRunStatus/@state"/>
    </xsl:attribute>
    <xsl:attribute name="jsxftpdate">
      <xsl:value-of select="$record_el/Status/FTPStatus/@date"/>
    </xsl:attribute>
    <xsl:attribute name="jsxftptime">
      <xsl:value-of select="$record_el/Status/FTPStatus/@time"/>
    </xsl:attribute>
    <xsl:attribute name="jsxftpstate">
      <xsl:value-of select="$record_el/Status/FTPStatus/@state"/>
    </xsl:attribute>   
  </xsl:template>

</xsl:stylesheet>