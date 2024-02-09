<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <DeviceConfig>
              <ServiceElementsConfig>
                <XMPPServiceConfig>
                      <IPVSDBService>
                        <xsl:attribute name="Dirty">
                            <xsl:value-of select="@DirtyBitblkScheduleBackup"/>
                        </xsl:attribute>
                        <Backup>
                        <DailyBackup>
                          <xsl:choose>
                            <xsl:when test="@jsxDailyBackup='true' or @jsxDailyBackup=1">Yes</xsl:when>
                            <xsl:otherwise>No</xsl:otherwise>
                          </xsl:choose>
                        </DailyBackup>
                        <BackupTime>
                            <xsl:value-of select="@jsxBackupTime"/>
                        </BackupTime>
                        <EnableExport>
                          <xsl:choose>
                            <xsl:when test="@jsxEnableExport='true' or @jsxEnableExport=1" >Yes</xsl:when>
                            <xsl:otherwise>No</xsl:otherwise>
                          </xsl:choose>
                        </EnableExport> 
                        <ExportURL>
                            <xsl:value-of select="@jsxExportURL"/>
                        </ExportURL>
                        </Backup>
                        <Restore>
                        <RecoveryMode>
                          <xsl:choose>
                            <xsl:when test="@jsxRecoveryMode='true' or @jsxRecoveryMode=1">LastBackup</xsl:when>
                            <xsl:otherwise>Factory</xsl:otherwise>
                          </xsl:choose>
                        </RecoveryMode>
                        </Restore> 
                      </IPVSDBService>
                  </XMPPServiceConfig>
              </ServiceElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
