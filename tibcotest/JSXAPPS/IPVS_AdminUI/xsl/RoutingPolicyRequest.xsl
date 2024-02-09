<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <MediaStreamFilter>
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <xsl:attribute name="parentNID">
              <xsl:value-of select="@parentNID"/>
          </xsl:attribute>
          <xsl:attribute name="Rev">
              <xsl:value-of select="@jsxStreamFilterXMLRevision"/>
          </xsl:attribute>
          <xsl:attribute name="Ver">
              <xsl:value-of select="@jsxStreamFilterXMLVersion"/>
          </xsl:attribute>
          <xsl:attribute name="serviceVer">
              <xsl:value-of select="@jsxStreamFilterXMLServiceVersion"/>
          </xsl:attribute>
          <Name>
              <xsl:value-of select="@jsxPolicyStreamFilterName"/>
          </Name> 
          <Description>
          </Description>
          <SrcFilterSet>
          </SrcFilterSet>
          <DstFilterSet>
          </DstFilterSet>
          <RelayFlag> 
          </RelayFlag>
          <RelayFilterSet>
          </RelayFilterSet>
          <PVRFilterSet>
          </PVRFilterSet>
          <ProfileFilterSet>
          </ProfileFilterSet>
          <RuleKeyTagFilterSet>
          </RuleKeyTagFilterSet>
        </MediaStreamFilter>
    </xsl:template>

 </xsl:stylesheet>
