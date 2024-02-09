<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <ServiceOperatorFilterSet>
        <xsl:apply-templates/>
     </ServiceOperatorFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <ServiceOperatorFilter>
          <UserFilterNID>
              <xsl:value-of select="@jsxtextOneNID"/>
          </UserFilterNID>
          <ResourceFilterNID>
              <xsl:value-of select="@jsxtextTwoNID"/>
          </ResourceFilterNID>
        </ServiceOperatorFilter>
    </xsl:template>

 </xsl:stylesheet>
