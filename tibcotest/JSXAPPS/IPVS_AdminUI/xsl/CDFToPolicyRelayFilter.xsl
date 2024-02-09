<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <RelayFilterSet>
        <xsl:apply-templates/>
     </RelayFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <RelayFilter>
          <UserFilterNID>
              <xsl:value-of select="@jsxtextOneNID"/>
          </UserFilterNID>
          <MediaStreamResourceFilterNID>
              <xsl:value-of select="@jsxtextTwoNID"/>
          </MediaStreamResourceFilterNID>
        </RelayFilter>
    </xsl:template>

 </xsl:stylesheet>
