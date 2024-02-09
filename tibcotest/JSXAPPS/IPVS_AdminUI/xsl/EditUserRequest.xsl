<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
        <User>
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
          <Groups>
          </Groups> 
          <Username>
              <xsl:value-of select="@Username"/>
          </Username> 
          <Password>
              <xsl:value-of select="@Password"/>
          </Password>
          <Email>
              <xsl:value-of select="@Email"/>
          </Email>
          <Name>
              <xsl:value-of select="@Name"/>
          </Name>
          <ExternalAuthentication>
                <xsl:choose>
                    <xsl:when test="@ExternalAuthentication='true' or @ExternalAuthentication=1" >Yes</xsl:when>
                    <xsl:otherwise>No</xsl:otherwise>
                </xsl:choose>
          </ExternalAuthentication>
          <CreationDate>
              <xsl:value-of select="@CreationDate"/>
          </CreationDate>
          <ModifiedDate>
              <xsl:value-of select="@ModifiedDate"/>
          </ModifiedDate>
          <UserPrivateData>
              <xsl:value-of select="@UserPrivateData"/>
          </UserPrivateData>
          <Roster>
            <Item askstatus="" jid="" name="" recvstatus="" substatus="">
              <Group /> 
            </Item>
          </Roster>

        </User>
    </xsl:template>

 </xsl:stylesheet>
