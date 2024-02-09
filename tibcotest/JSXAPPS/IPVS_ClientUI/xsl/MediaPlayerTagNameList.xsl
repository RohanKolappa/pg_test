<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="GetPortResponse">
        <data jsxid="jsxroot">
         <record jsxid="All" jsxtext="All">
            <Tag>
             <xsl:apply-templates mode="alltagvalues" select="/GetPortResponse/Port/Groups/Tag" />
            </Tag>
          </record>
          <xsl:apply-templates select="*"/>  
        </data>
    </xsl:template>

    <xsl:template match="GetPortResponse/Port/Groups">
        <record>
            <xsl:attribute name="jsxid">
                <xsl:value-of select="./Tag/@Name"/>
            </xsl:attribute>
            <xsl:attribute name="jsxtext">
                <xsl:value-of select="./Tag/@Name"/>
            </xsl:attribute>
            <Tag>
             <xsl:apply-templates mode="tagvalues" select="Tag" /> 
            </Tag>
        </record>
    </xsl:template>

    <xsl:template mode="alltagvalues" match="/GetPortResponse/Port/Groups/Tag">
      <xsl:choose>
        <xsl:when test="string-length(normalize-space(@Name))=0">
        </xsl:when>
        <xsl:otherwise>
           <Value>
             <xsl:attribute name="jsxTagName">
               <xsl:value-of select="@Name"/>
             </xsl:attribute>           
            <xsl:value-of select="@Value"/>
           </Value>
        </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

    <xsl:template mode="tagvalues" match="/GetPortResponse/Port/Groups/Tag">
      <xsl:choose>
        <xsl:when test="string-length(normalize-space(@Name))=0">
        </xsl:when>
        <xsl:otherwise>
           <Value>
             <xsl:attribute name="jsxTagName">
               <xsl:value-of select="@Name"/>
             </xsl:attribute>
             <xsl:value-of select="@Value"/>
           </Value>
        </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

</xsl:stylesheet>
