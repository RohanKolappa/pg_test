<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <AssetResource>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@Rev"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@Ver"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@serviceVer"/>
          </xsl:attribute>
          <xsl:attribute name="Persistence">
              <xsl:value-of select="@Persistence"/>
          </xsl:attribute>
          <Contact>
            <Presence>
                 <xsl:choose>
                  <xsl:when test="@Presence='1'">unmanaged</xsl:when>
                  <xsl:when test="@Presence='0'"></xsl:when>
                </xsl:choose>
            </Presence>
            <OwnerUserJID>
                <xsl:value-of select="@OwnerUserJID"/>
            </OwnerUserJID>
          </Contact>
          <State>
            <StateFlag>
                <xsl:choose>
                  <xsl:when test="@StateFlag='Offline'">Offline</xsl:when>
                  <xsl:when test="@StateFlag='Ready'">Ready</xsl:when>
                  <xsl:when test="@StateFlag='NotReady'">NotReady</xsl:when>
                  <xsl:when test="@StateFlag='Idle'">Idle</xsl:when>
                  <xsl:when test="@StateFlag='Reserved'">Reserved</xsl:when>
                  <xsl:when test="@StateFlag='Busy'">Busy</xsl:when>
                  <xsl:when test="@StateFlag='Unmanaged'">Unmanaged</xsl:when>
                  <xsl:when test="@StateFlag='Template'">Template</xsl:when>
                  <xsl:otherwise>Error</xsl:otherwise>
                </xsl:choose>
            </StateFlag>
            <StateMsg>
                <xsl:value-of select="@StateMsg"/>
            </StateMsg>
          </State>
          <TemplateAssetResourceNID>
            <xsl:value-of select="@TemplateAssetResourceNID"/>
          </TemplateAssetResourceNID>
          <Info>
              <Groups>
              </Groups> 
              <Title>
                  <xsl:value-of select="@DeviceName"/>
              </Title> 
              <Description>
                  <xsl:value-of select="@DeviceDescription"/>
              </Description>
              <Type>
                  <xsl:value-of select="@DeviceType"/>
              </Type>
              <AssetPrivateKey>
                  <KeyScheme>
                      <xsl:value-of select="@KeyScheme"/>
                  </KeyScheme>
                  <KeyData>
                      <xsl:value-of select="@Password"/>
                  </KeyData>
              </AssetPrivateKey>
              <AssetPrivateData>
                  <xsl:value-of select="@AssetPrivateData"/>
              </AssetPrivateData>
              <FeatureList>
                 <Feature></Feature>
              </FeatureList>
           </Info>
        </AssetResource>
    </xsl:template>

 </xsl:stylesheet>
