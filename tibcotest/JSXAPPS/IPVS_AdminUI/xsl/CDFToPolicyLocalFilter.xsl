<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <LocalNetworkResourceFilterSet>
        <xsl:apply-templates/>
     </LocalNetworkResourceFilterSet> 
    </xsl:template>

    <xsl:template match="record">
          <LocalNetworkResourceFilter>
            <MediaStreamResourceFilterNID><xsl:value-of select="@jsxtextOneNID"/></MediaStreamResourceFilterNID>
          </LocalNetworkResourceFilter>
    </xsl:template>

 </xsl:stylesheet>
