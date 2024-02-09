<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="GetBookmark">
       <data jsxid="jsxroot">
            <xsl:apply-templates/>
       </data>
    </xsl:template> 
   
    <xsl:template match="GetBookmark/Bookmark">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
          <xsl:copy-of select="MetaData"> </xsl:copy-of>
       </record>
    </xsl:template>    
 
   <xsl:template name="record_attributes">
      <xsl:param name="record_el"/>
      <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>
      <xsl:attribute name="jsxtext"><xsl:value-of select="$record_el/@title"/></xsl:attribute>
      <xsl:attribute name="bookmarkTS"><xsl:value-of select="$record_el/@TC"/></xsl:attribute>
      <xsl:attribute name="jsxOffset"><xsl:value-of select="$record_el/@offset"/></xsl:attribute>
      <xsl:attribute name="jsxDateCreated"><xsl:value-of select="$record_el/@dateCreated"/></xsl:attribute>
      <xsl:attribute name="jsxOwner"><xsl:value-of select="$record_el/@owner"/></xsl:attribute>
      <xsl:attribute name="jsxMediaStreamFileNID"><xsl:value-of select="$record_el/@mediaStreamFileNID"/></xsl:attribute>
      <xsl:attribute name="jsxDescription"><xsl:value-of select="$record_el/Description"/></xsl:attribute>
   </xsl:template>
 </xsl:stylesheet>