<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <TagObjectTypeList>
        <xsl:apply-templates/>
     </TagObjectTypeList> 
    </xsl:template>

    <xsl:template match="record">
          <TagObjectType>
            <xsl:value-of select="@jsxroleObject"/>
          </TagObjectType>
    </xsl:template>
 </xsl:stylesheet>
