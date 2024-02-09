<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="GetPortTags">
        <data jsxid="jsxroot">
         <record jsxid="All" jsxtext="All">
            <Tag>
             <xsl:apply-templates mode="alltagvalues" select="/GetPortTags/Tag/Value" />
            </Tag>
          </record>
          <xsl:apply-templates/>  
        </data>
    </xsl:template>

    <xsl:template match="GetPortTags/Tag">
        <record>
            <xsl:attribute name="jsxid">
                <xsl:value-of select="@Name"/>
            </xsl:attribute>
            <xsl:attribute name="jsxtext">
                <xsl:value-of select="@Name"/>
            </xsl:attribute>
            <Tag>
             <xsl:apply-templates mode="tagvalues" select="Value" /> 
            </Tag>
        </record>
    </xsl:template>

    <xsl:template mode="alltagvalues" match="/GetPortTags/Tag/Value">
      <xsl:choose>
        <xsl:when test="string-length(normalize-space(.))=0">
        </xsl:when>
        <xsl:otherwise>
           <Value>
             <xsl:attribute name="jsxTagName">
               <xsl:value-of select="../@Name"/>
             </xsl:attribute>           
            <xsl:value-of select="."/>
           </Value>
        </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

    <xsl:template mode="tagvalues" match="/GetPortTags/Tag/Value">
      <xsl:choose>
        <xsl:when test="string-length(normalize-space(.))=0">
        </xsl:when>
        <xsl:otherwise>
           <Value>
             <xsl:attribute name="jsxTagName">
               <xsl:value-of select="../@Name"/>
             </xsl:attribute>
             <xsl:value-of select="."/>
           </Value>
        </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

</xsl:stylesheet>





