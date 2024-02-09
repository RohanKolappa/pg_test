<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <RuleKeyTagFilterSet>
        <xsl:apply-templates/>
     </RuleKeyTagFilterSet> 
    </xsl:template>

    <xsl:template match="record">
        <RuleKeyTagFilter>
          <RuleKeyLHTagName>
              <RuleKey>
                  <xsl:value-of select="@jsxtextLHRuleKey"/>
              </RuleKey>
              <TagName>//Groups/Tag/@Name='<xsl:value-of select="@jsxtextLHTagName"/>' and //Groups/Tag/@Value</TagName>
          </RuleKeyLHTagName>
          <MatchOp>
              <xsl:value-of select="@jsxtextOperation"/>
          </MatchOp>
          <RuleKeyRHTagName>
              <RuleKey>
                  <xsl:value-of select="@jsxtextRHRuleKey"/>
              </RuleKey>
              <TagName>//Groups/Tag/@Name='<xsl:value-of select="@jsxtextRHTagName"/>' and //Groups/Tag/@Value</TagName>
          </RuleKeyRHTagName>
        </RuleKeyTagFilter>
    </xsl:template>

 </xsl:stylesheet>
