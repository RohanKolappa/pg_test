<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <ProfileFilterSet>
        <xsl:apply-templates/>
     </ProfileFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <ProfileFilter>
          <MediaStreamProfileFilterNID><xsl:value-of select="@jsxtextOneNID"/></MediaStreamProfileFilterNID>
         </ProfileFilter>
    </xsl:template>

 </xsl:stylesheet>
