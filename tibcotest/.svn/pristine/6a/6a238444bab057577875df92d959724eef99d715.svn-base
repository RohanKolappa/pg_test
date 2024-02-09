<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <TagFilterSet>
        <xsl:apply-templates/>
     </TagFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <TagFilter>
          <LHTagName>//Groups/Tag/@Name='<xsl:value-of select="@jsxtextOne"/>' and //Groups/Tag/@Value</LHTagName>
          <MatchOp>
              <xsl:value-of select="@jsxtextOperation"/>
          </MatchOp>
          <RHRegexp>
              <xsl:value-of select="@jsxtextTwo"/>
          </RHRegexp>
        </TagFilter>
    </xsl:template>
 </xsl:stylesheet>
