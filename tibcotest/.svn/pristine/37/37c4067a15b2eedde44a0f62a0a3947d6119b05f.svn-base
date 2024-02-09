<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>

  <xsl:template match="/">
          <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetUserResponse">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="GetUserResponse/User">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
    </record>
  </xsl:template> 


  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@username"/>
    </xsl:attribute>
    <xsl:attribute name="jsxUserFullName">
      <xsl:value-of select="$record_el/@fullName"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextuserEmail">
      <xsl:value-of select="$record_el/@email"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtextuserCreated">
      <xsl:value-of select="$record_el/@creationDate"/>
    </xsl:attribute>
    <xsl:attribute name="jsxType">User</xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/Groups">
        <xsl:for-each select="$record_el/Groups/Tag">
          <xsl:sort select="@Name"/>
          <xsl:variable name="tagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
          <xsl:variable name="tagValue"><xsl:value-of select="@Value"/></xsl:variable>
          <xsl:if test="$tagName!=''">
            <xsl:variable name="tagAA"><xsl:value-of select="concat('jsxTagValue',$tagName)"/></xsl:variable>
            <!--
            <xsl:if test="position()=1 or (position()>1 and following-sibling::Tag/@Name != $tagName) or(position()=last() and preceding-sibling::Tag/@Name = $tagName) or (position()=last() and preceding-sibling::Tag/@Name != $tagName)">
            -->
            <xsl:if test="position()=1 or (position()>1) ">
              <xsl:attribute name="{$tagAA}">
                <xsl:variable name="ConcatedTagValues">
                  <xsl:for-each select="$record_el/Groups/Tag">
                    <xsl:sort select="@Name"/>
                    <xsl:variable name="sortTagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
                    <xsl:if test="$tagName=$sortTagName">
                      <xsl:value-of select="concat(',',@Value)"/>
                    </xsl:if>
                  </xsl:for-each>
                </xsl:variable>
                <xsl:value-of select="substring-after($ConcatedTagValues, ',')"/>
              </xsl:attribute>
            </xsl:if>
          </xsl:if>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
      </xsl:otherwise>
    </xsl:choose>  

    <xsl:choose>
      <xsl:when test="$record_el/@presence='available'">
        <xsl:attribute name="jsxUserStatus">images/icons/icon_user_ready.png</xsl:attribute>
        <xsl:attribute name="jsxStateFlag">
          <xsl:value-of select="$record_el/@state"/>
        </xsl:attribute>
        <xsl:attribute name="jsxPresence">
          <xsl:value-of select="$record_el/@presence"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxUserStatus">images/icons/icon_user_offline.png</xsl:attribute>
        <xsl:attribute name="jsxStateFlag"></xsl:attribute>
        <xsl:attribute name="jsxPresence"></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>