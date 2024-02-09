<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="jsxid">
              <xsl:value-of select="@jsxid"/>
          </xsl:attribute>
          <DeviceConfig>
              <ServiceElementsConfig>
                <XMPPServiceConfig>
                      <XMPPServer>
                         <XMPPServerLDAPAuth>
                            <xsl:attribute name="Dirty">
                                <xsl:value-of select="@DirtyBitExternalAuth"/>
                            </xsl:attribute>
                            <Enable>
                                <xsl:choose>
                                    <xsl:when test="@EnableExternalAuth='true' or @EnableExternalAuth=1" >Yes</xsl:when>
                                    <xsl:otherwise>No</xsl:otherwise>
                                </xsl:choose>
                            </Enable>
                            <Host>
                                <xsl:value-of select="@ExternalAuthServer"/>
                            </Host>
                            <AdminDN>
                                <xsl:value-of select="@ExternalAuthAdminDN"/>
                            </AdminDN>
                            <AdminPassword>
                                <xsl:value-of select="@ExternalAuthPassword"/>
                            </AdminPassword>
                            <BaseDN>
                                <xsl:value-of select="@ExternalAuthBaseDN"/>
                            </BaseDN>
                            <UsernameField>
                                <xsl:value-of select="@ExternalAuthUserNameField"/>
                            </UsernameField>
                        </XMPPServerLDAPAuth>
                      </XMPPServer>
                  </XMPPServiceConfig>
              </ServiceElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
