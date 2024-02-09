<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>

  <xsl:template match="/">
    <data jsxid="jsxroot">
    <record jsxid="All" jsxtext="All">
      <Tag>
      <xsl:call-template name="getgrouptagvaluesU">
       <xsl:with-param name="parent_el" select="."/>
      </xsl:call-template> 
      </Tag>
    </record>
    <xsl:call-template name="getgrouptagvaluesM">
     <xsl:with-param name="parent_el" select="."/>
    </xsl:call-template> 
    </data>
  </xsl:template>
   <xsl:template match="*"/>


  <xsl:template name="getgrouptagvaluesU">
    <xsl:param name="parent_el"/>
    <xsl:for-each select="$parent_el//Tag">
      <xsl:variable select="concat(@Value, @Name)" name="toptag" />
      <xsl:if test="( not(preceding-sibling::Tag[$toptag=concat(@Value, @Name)] ) ) ">
        <Value>
          <xsl:attribute name="jsxTagName">
            <xsl:value-of select="@Name"/>
          </xsl:attribute>           
          <xsl:value-of select="./@Value"/>
        </Value>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>
   

  <xsl:template name="getgrouptagvaluesM">
  <xsl:param name="parent_el"/>
    <xsl:for-each select="$parent_el//Tag">
      <xsl:variable select="@Name" name="toptag" />
      <xsl:if test="( not(preceding-sibling::Tag[@Name=$toptag] ) ) ">
        <record>
          <xsl:attribute name="jsxid">
            <xsl:value-of select="$toptag"/>
          </xsl:attribute>           
          <xsl:attribute name="jsxtext">
            <xsl:value-of select="$toptag"/>
          </xsl:attribute>   
          <Tag>
            <!--  do one more loop on unique names to see there are more than one siblng-->
            <xsl:choose>
              <xsl:when test="count($parent_el//Tag[@Name=$toptag]) > 1 ">
                <xsl:for-each select="$parent_el//Tag[@Name=$toptag]">
                  <xsl:variable select="concat(@Value, @Name)" name="innerValue" />
                  <xsl:if test="( not(preceding-sibling::Tag[$innerValue=concat(@Value, @Name)] ) ) ">
                    <Value>
                      <xsl:attribute name="jsxTagName">
                        <xsl:value-of select="@Name"/>
                      </xsl:attribute>           
                      <xsl:value-of select="./@Value"/>
                    </Value>
                  </xsl:if>
                </xsl:for-each>
              </xsl:when>
              <xsl:otherwise>
                <Value>
                  <xsl:attribute name="jsxTagName">
                    <xsl:value-of select="@Name"/>
                  </xsl:attribute>           
                  <xsl:value-of select="./@Value"/>
                </Value>
              </xsl:otherwise>
            </xsl:choose>
          </Tag>
        </record>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

 

  
   
</xsl:stylesheet>
