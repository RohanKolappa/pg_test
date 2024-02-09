<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <RemoteNetworkResourceFilterSet>
        <xsl:apply-templates/>
     </RemoteNetworkResourceFilterSet> 
    </xsl:template>

    <xsl:template match="record">
          <RemoteNetworkResourceFilter>
            <MediaStreamResourceFilterNID><xsl:value-of select="@jsxtextOneNID"/></MediaStreamResourceFilterNID>
          </RemoteNetworkResourceFilter>
    </xsl:template>

 </xsl:stylesheet>
