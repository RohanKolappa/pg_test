<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <MediaStoreAdminObjectFilterSet>
        <xsl:apply-templates/>
     </MediaStoreAdminObjectFilterSet> 
    </xsl:template>

    <xsl:template match="record">
          <MediaStoreAdminObjectFilterNID><xsl:value-of select="@jsxtextOneNID"/></MediaStoreAdminObjectFilterNID>
    </xsl:template>

 </xsl:stylesheet>
