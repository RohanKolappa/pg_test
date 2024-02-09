<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="GetSession">
       <data jsxid="jsxroot">
            <xsl:apply-templates/>
       </data>
    </xsl:template> 
   
    <xsl:template match="GetSession/Session">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
       </record>
    </xsl:template>    

   <xsl:template name="record_attributes">
           <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@id"/></xsl:attribute>
          <xsl:attribute name="jsxLiveSessionConnectionID"><xsl:value-of select="$record_el/@id"/></xsl:attribute>
          <xsl:attribute name="jsxLiveSessionName"><xsl:value-of select="$record_el/@name"/></xsl:attribute>
	  <xsl:attribute name="jsxLiveSessionCreated"><xsl:value-of select="$record_el/Properties/@dateCreated"/></xsl:attribute>
	  <xsl:attribute name="jsxLiveSessionState"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
	  <xsl:attribute name="jsxLiveSessionOwnerJID"><xsl:value-of select="$record_el/Properties/@ownerUserJID"/></xsl:attribute>
   </xsl:template>
</xsl:stylesheet>