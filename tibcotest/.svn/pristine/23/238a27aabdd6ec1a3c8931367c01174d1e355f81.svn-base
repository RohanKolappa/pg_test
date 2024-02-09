<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:apply-templates/>
    </xsl:template>

  <xsl:template match="GetSystemConfig">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>
   
  <xsl:template match="GetSystemConfig/Device">
        <record>
            <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>
  
    <xsl:template name="record_attributes">
      <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
          <xsl:value-of select="@jid"/>
        </xsl:attribute>
        <xsl:attribute name="jsxnid">
          <xsl:value-of select="@NID"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitblkScheduleBackup">
          <xsl:value-of select="$record_el/ScheduleBackup/@DirtyBitblkScheduleBackup"/>
        </xsl:attribute>
        <xsl:attribute name="jsxDailyBackup">
          <xsl:value-of select="$record_el/ScheduleBackup/@enable"/>
        </xsl:attribute>
      <xsl:attribute name="jsxBackupTime">
          <xsl:value-of select="$record_el/ScheduleBackup/@backupTime"/>
      </xsl:attribute>
        <xsl:attribute name="jsxEnableExport">
          <xsl:value-of select="$record_el/ScheduleBackup/@enableExport"/>
        </xsl:attribute>
        <xsl:attribute name="jsxExportURL">
          <xsl:value-of select="$record_el/ScheduleBackup/@exportURL"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/ScheduleBackup/@recoveryMode ='LastBackup'">
                <xsl:attribute name="jsxRecoveryMode">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxRecoveryMode">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
     </xsl:template>
</xsl:stylesheet>