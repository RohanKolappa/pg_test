<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <xsl:call-template name="exAuthTemplate">
        <xsl:with-param name="exAuth_el" select="/*" />
      </xsl:call-template>
    </xsl:template>
   
    <xsl:template name="exAuthTemplate">
      <xsl:param name="exAuth_el"/>
      <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="$exAuth_el"/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>
  
  <xsl:template name="record_attributes">
      <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/Device/@jid"/>
        </xsl:attribute>
        <xsl:attribute name="jsxnid">
            <xsl:value-of select="$record_el/Device/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="DirtyBitExternalAuth">
            <xsl:value-of select="@DirtyBitExternalAuth"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthServer">
           <xsl:value-of select="$record_el/Device/ExternalAuthentication/@host"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthAdminDN">
          <xsl:value-of select="$record_el/Device/ExternalAuthentication/@adminDN"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthPassword">
          <xsl:value-of select="$record_el/Device/ExternalAuthentication/@adminPassword"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthBaseDN">
          <xsl:value-of select="$record_el/Device/ExternalAuthentication/@baseDN"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthUserNameField">
          <xsl:value-of select="$record_el/Device/ExternalAuthentication/@usernameField"/>
        </xsl:attribute>
        <xsl:attribute name="EnableExternalAuth">
            <xsl:value-of select="$record_el/Device/ExternalAuthentication/@enable"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>