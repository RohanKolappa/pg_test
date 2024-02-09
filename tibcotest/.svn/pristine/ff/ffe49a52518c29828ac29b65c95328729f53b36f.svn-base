<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <SrcFilterSet>
        <xsl:apply-templates/>
     </SrcFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <SrcFilter>
          <UserFilterNID>
              <xsl:value-of select="@jsxtextOneNID"/>
          </UserFilterNID>
          <MediaStreamResourceFilterNID>
              <xsl:value-of select="@jsxtextTwoNID"/>
          </MediaStreamResourceFilterNID>
        </SrcFilter>
    </xsl:template>

 </xsl:stylesheet>
