<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="GetUser">
       <data jsxid="jsxroot">
            <xsl:apply-templates/>
       </data>
    </xsl:template> 
   
    <xsl:template match="GetUser/User[Groups/Tag/@Name!='_RESERVED' and Groups/Tag/@Value!='DEVICE_BOT_USERS']">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
       </record>
    </xsl:template>    

   <xsl:template name="record_attributes">
          <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>
          <xsl:attribute name="jsxtext"><xsl:value-of select="$record_el/@title"/></xsl:attribute>
         <xsl:attribute name="jsxUserJID"><xsl:value-of select="$record_el/@jid"/></xsl:attribute>
         <xsl:attribute name="jsxUserDescription"><xsl:value-of select="$record_el/description"/></xsl:attribute>
         <xsl:attribute name="jsxUserPresence"><xsl:value-of select="$record_el/@presence"/></xsl:attribute>
	       
	       <xsl:choose>
            <xsl:when test="$record_el/@presence='available'">
                  <xsl:attribute name="jsxUserPresence">images/icons/icon_greenBall.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@presence=''">
                  <xsl:attribute name="jsxUserPresence">images/icons/icon_lightgrayBall.png</xsl:attribute>
            </xsl:when>            
            <xsl:when test="$record_el/@presence='unavailable'">
                  <xsl:attribute name="jsxUserPresence">images/icons/icon_grayBall.png</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="jsxUserPresence">images/icons/icon_grayBall.png</xsl:attribute>
            </xsl:otherwise>
         </xsl:choose>

	      <xsl:attribute name="jsxUserTags">
		      <xsl:variable name="strTags">
		        <xsl:for-each select="$record_el/Groups/Tag">
		        <xsl:text>,</xsl:text><xsl:value-of select="@Name" />=<xsl:value-of select="@Value" />
		        </xsl:for-each>
		      </xsl:variable>
		      <xsl:value-of select="substring-after($strTags, ',')"/>
        </xsl:attribute>
    </xsl:template>
</xsl:stylesheet>