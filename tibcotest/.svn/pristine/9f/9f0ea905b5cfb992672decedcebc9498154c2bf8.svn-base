<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="Session">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="Session/EntityList/Entity[@type='SrcPort' or @type='PlyFile'] ">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:variable name="enity_el" select="."/>
      <xsl:for-each select="//Connection[@srcEntityId=$enity_el/@id and (@type='VIEW' or @type='PLAYBACK')]">
        <xsl:variable name="connection_el" select="."/>
        <xsl:variable name="enity_dest_el" select="//Entity[@id=$connection_el/@dstEntityId]"/>
        <xsl:call-template name="getdestination">
          <xsl:with-param name="enity_el" select="$enity_el"/>
          <xsl:with-param name="connection_el" select="$connection_el"/>
          <xsl:with-param name="enity_dest_el" select="$enity_dest_el"/>
        </xsl:call-template>
      </xsl:for-each>
      <Groups></Groups>
    </record>    
  </xsl:template> 

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:param name="NIDParam"/>
    <xsl:attribute name="jsxid">
    <xsl:choose>
      <xsl:when test="$record_el/File">
        <xsl:value-of select="$record_el/File/@resourceNID"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$record_el/Port[@resourceNID !='']/@resourceNID"/>
      </xsl:otherwise>
    </xsl:choose>
    </xsl:attribute>
    <xsl:attribute name="jsxEntityid">
      <xsl:value-of select="$record_el/@id"/>
    </xsl:attribute>
    <xsl:attribute name="jsxEntityType">
      <xsl:value-of select="$record_el/@type"/>
    </xsl:attribute>
    <xsl:attribute name="jsxVisible">0</xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@name"/>
    </xsl:attribute>   
    <xsl:attribute name="jsxpresence">
      <xsl:value-of select="$record_el/@presence"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/@presence='available'">
        <xsl:attribute name="jsxdevpresence">JSXAPPS/IPVS_ClientUI/images/icons/icon_greenBall.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@presence=''">
        <xsl:attribute name="jsxdevpresence">JSXAPPS/IPVS_ClientUI/images/icons/icon_lightgrayBall.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@presence='unavailable'">
        <xsl:attribute name="jsxdevpresence">JSXAPPS/IPVS_ClientUI/images/icons/icon_grayBall.png</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxdevpresence">JSXAPPS/IPVS_ClientUI/images/icons/icon_grayBall.png</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:attribute name="jsxdevStreamType">
      <xsl:value-of select="$record_el/@streamType"/>
    </xsl:attribute>

  </xsl:template>


  <xsl:template name="getdestination">
    <xsl:param name="enity_el"/>
    <xsl:param name="connection_el"/>
    <xsl:param name="entity_name"/>
    <xsl:param name="enity_dest_el"/>
    <destination>
      <xsl:attribute name="jsxdestinationid">
        <xsl:value-of select="$connection_el/@dstEntityId"/>
      </xsl:attribute>
      <xsl:attribute name="jsxwindowid">
        <xsl:value-of select="$connection_el/@windowId"/>
      </xsl:attribute>
      <xsl:attribute name="jsxconnectionstate">
        <xsl:value-of select="$connection_el/@state"/>
      </xsl:attribute>
      <xsl:attribute name="jsxVisible">1</xsl:attribute>
      <xsl:choose>
        <xsl:when test="($connection_el/@state = 'STREAMING')">
          <xsl:attribute name="jsxtstate">JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="jsxtstate">JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>      
      <xsl:choose>
        <xsl:when test="($connection_el/@state = 'SETUP_ERROR') or ($connection_el/@state ='START_ERROR') or ($connection_el/@state ='SOURCE_ERROR') or ($connection_el/@state ='MEDIA_ERROR')or ($connection_el/@state ='DEST_ERROR')">
          <xsl:attribute name="jsxerrorimg">JSXAPPS/IPVS_ClientUI/images/icons/icon_error_small.png</xsl:attribute>
          <xsl:attribute name="jsxerrorcode">
            <xsl:value-of select="$connection_el/@errorCode"/>
          </xsl:attribute>
          <xsl:attribute name="jsxerrordata">
            <xsl:value-of select="$connection_el/@errorData"/>
          </xsl:attribute>
        </xsl:when>
      </xsl:choose>
      <xsl:attribute name="jsxstreamURL">
        <xsl:value-of select="$connection_el/@streamURL"/>
      </xsl:attribute>
      <xsl:attribute name="jsxconnectionid">
        <xsl:value-of select="$connection_el/@id"/>
      </xsl:attribute>
      <xsl:attribute name="jsxdestinationname">
        <xsl:value-of select="$enity_dest_el/@name"/>
      </xsl:attribute>  
    </destination> 
  </xsl:template>
</xsl:stylesheet>