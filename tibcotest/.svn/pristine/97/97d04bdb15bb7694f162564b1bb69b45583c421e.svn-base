<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="GetSessionConfig">
       <data jsxid="jsxroot">
            <xsl:apply-templates/>
       </data>
    </xsl:template> 
   
    <xsl:template match="GetSessionConfig/SessionConfig">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
       </record>
    </xsl:template>    

   <xsl:template name="record_attributes">
          <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>
          <xsl:attribute name="jsxCfgSessionName"><xsl:value-of select="Title"/></xsl:attribute>
          <xsl:attribute name="jsxCfgSessionCreated"><xsl:value-of select="Properties/DateCreated"/></xsl:attribute>
          <xsl:attribute name="jsxCfgSessionPlaybackOpt"><xsl:value-of select="$record_el/Settings/Playback/@recVersion"/></xsl:attribute>
          <xsl:choose>
            <xsl:when test="$record_el/Settings/Playback/@recVersion='DEFAULT'">
                  <xsl:attribute name="jsxCfgSessionPlaybackOpt">Default</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/Settings/Playback/@recVersion='LATEST'">
                  <xsl:attribute name="jsxCfgSessionPlaybackOpt">Latest</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/Settings/Playback/@recVersion='ADHOC'">
              <xsl:attribute name="jsxCfgSessionPlaybackOpt">Ad-Hoc</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="jsxCfgSessionPlaybackOpt">---</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:attribute name="jsxCfgTags">
            <xsl:variable name="strTags">
            <xsl:for-each select="Groups/Tag">
            <xsl:if test="string-length(@Name)!=0">
                <xsl:text>,</xsl:text><xsl:value-of select="@Name" />=<xsl:value-of select="@Value" />
            </xsl:if>
            </xsl:for-each>
            </xsl:variable>
            <xsl:value-of select="substring-after($strTags, ',')"/>
          </xsl:attribute>
   </xsl:template>
</xsl:stylesheet>