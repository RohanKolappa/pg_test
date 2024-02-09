<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetPortResponse/Port">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:apply-templates select="ConnectionStatus/PortSessionInfo"></xsl:apply-templates>
    </record>
  </xsl:template>


  <xsl:template match="GetPortResponse/Port/ConnectionStatus/PortSessionInfo">
    <PortSessionInfo>
      <xsl:call-template name="session_attributes">
        <xsl:with-param name="session_el" select="."/>
      </xsl:call-template>
      <xsl:apply-templates select="PortConnectionInfo"></xsl:apply-templates>
    </PortSessionInfo>
  </xsl:template>


  <xsl:template match="GetPortResponse/Port/ConnectionStatus/PortSessionInfo/PortConnectionInfo">
    <PortConnectionInfo>
      <xsl:call-template name="connection_attributes">
        <xsl:with-param name="connection_el" select="."/>
        <xsl:with-param name="record_el" select="/GetPortResponse/Port"/>        
      </xsl:call-template>
    </PortConnectionInfo>
  </xsl:template>

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>   
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>
    <xsl:attribute name="jsxportType">
      <xsl:value-of select="$record_el/@portType"/>
    </xsl:attribute>
    <xsl:choose>
    <xsl:when test="$record_el/@portType = 'StreamSrc'">
        <xsl:attribute name="jsxunselectable">1</xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:attribute name="jsxread">
      <xsl:value-of select="$record_el/Permissions/@read"/>
    </xsl:attribute>
    <xsl:attribute name="jsxstreamto">
      <xsl:value-of select="$record_el/Permissions/@streamto"/>
    </xsl:attribute>
    <xsl:attribute name="jsxmaxInstanceCount">
      <xsl:value-of select="MaxInstanceCount"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="MaxInstanceCount = '1'">
        <xsl:attribute name="jsxdevimg">jsxaddin://user!ipvsCommon/images/icons/pc_screen.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="MaxInstanceCount = '16'">
        <xsl:attribute name="jsxdevimg">jsxaddin://user!ipvsCommon/images/icons/computer.png</xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:attribute name="jsxdevstate">
      <xsl:value-of select="$record_el/@state"/>
    </xsl:attribute>   
    <xsl:attribute name="jsxpresence">
      <xsl:value-of select="$record_el/@presence"/>
    </xsl:attribute> 
    <xsl:attribute name="jsxdevStreamType">
      <xsl:value-of select="$record_el/@streamType"/>
    </xsl:attribute>
    <xsl:attribute name="jsxdevicenid">
      <xsl:value-of select="Asset/@deviceNID"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="(FeatureList/Feature/@name = 'XP200DisplayConfig')or(FeatureList/Feature/@name = 'IPVSClientDisplayConfig')" >
	    <xsl:attribute name="jsxfeature">
	      <xsl:value-of select="FeatureList/Feature/@name"/>
	    </xsl:attribute>
      </xsl:when>
      <xsl:otherwise><xsl:attribute name="jsxfeature"> </xsl:attribute></xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="(FeatureList/Feature/@name = 'XP200DisplayConfig')or(FeatureList/Feature/@name = 'IPVSClientDisplayConfig')" >
      <xsl:attribute name="jsxfeaturename">jsxaddin://user!ipvsCommon/images/buttons/btnWindowingControllerOff.png</xsl:attribute>
      </xsl:when>
    </xsl:choose>
      <xsl:attribute name="jsxdevTags">
      <xsl:variable name="strTags">
        <xsl:for-each select="Groups/Tag">
          <xsl:text>,</xsl:text><xsl:value-of select="@Name" />=<xsl:value-of select="@Value" />
        </xsl:for-each>
      </xsl:variable>
      <xsl:value-of select="substring-after($strTags, ',')"/>
    </xsl:attribute>   
  </xsl:template>

  <xsl:template name="session_attributes">
    <xsl:param name="session_el"/>
    <xsl:attribute name="jsxsessionid">
      <xsl:value-of select="$session_el/@id"/>
    </xsl:attribute>
    <xsl:attribute name="jsxsessionname">
      <xsl:value-of select="$session_el/@name"/>
    </xsl:attribute>  
  </xsl:template>
  

  <xsl:template name="connection_attributes">
    <xsl:param name="connection_el"/>
    <xsl:param name="record_el"/>
    <xsl:attribute name="jsxconnectionid">
      <xsl:value-of select="$connection_el/@id"/>
    </xsl:attribute>
    <xsl:attribute name="jsxconnectionname">
      <xsl:value-of select="$connection_el/@name"/>
    </xsl:attribute>
    <xsl:attribute name="jsxsrcnid">
      <xsl:value-of select="$connection_el/@srcNID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxdstnid">
      <xsl:value-of select="$connection_el/@dstNID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxconnectionstate">
      <xsl:value-of select="$connection_el/@state"/>
    </xsl:attribute>
      <xsl:attribute name="jsxwindowid">
      <xsl:value-of select="$connection_el/@windowId"/>
    </xsl:attribute>
    <xsl:attribute name="jsxcanvasid">
      <xsl:value-of select="$connection_el/@canvasId"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="($connection_el/@state = 'SETUP_ERROR') or ($connection_el/@state ='START_ERROR') or ($connection_el/@state ='SOURCE_ERROR') or ($connection_el/@state ='MEDIA_ERROR')or ($connection_el/@state ='DEST_ERROR')">
        <xsl:attribute name="jsxerrorimg">images/icons/icon_error_small.png</xsl:attribute>
        <xsl:attribute name="jsxerrorcode">
          <xsl:value-of select="$connection_el/@errorCode"/>
        </xsl:attribute>
        <xsl:attribute name="jsxerrordata">
          <xsl:value-of select="$connection_el/@errorData"/>
        </xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="not($record_el/FeatureList/Feature/@name = 'IPVSClientDisplayConfig')">
        <xsl:attribute name="jsxunselectable">1</xsl:attribute>
      </xsl:when>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="feature_attributes">
    <xsl:param name="feature_el"/>
    <xsl:attribute name="jsxname">
      <xsl:value-of select="$feature_el/@name"/>
    </xsl:attribute>  
  </xsl:template>
  
</xsl:stylesheet>