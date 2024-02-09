<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:gicache="http://xsd.tns.tibco.com/gi/cache">
  <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
  <xsl:param name="NIDArgument"></xsl:param>

  <xsl:template match="/">
    <xsl:choose>
      <xsl:when test="name(/*)='loading' or name(/*)='timeout' or name(/*)='error'">
        <xsl:copy-of select="."/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="GetPortResponse">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>

  <xsl:template match="GetPortResponse/Port">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
      <xsl:apply-templates select="ConnectionStatus/Session/Connection"></xsl:apply-templates>
    </record>
  </xsl:template> 

  <xsl:template match="GetPortResponse/Port/ConnectionStatus/Session/Connection">
    <connection>
      <xsl:call-template name="connection_attributes">
        <xsl:with-param name="connection_el" select="."/>
        <xsl:with-param name="NIDParam" select="$NIDArgument"/>
      </xsl:call-template>
    </connection>
  </xsl:template>

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:param name="NIDParam"/>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>
    <xsl:attribute name="jsxdevstate">
      <xsl:value-of select="$record_el/@state"/>
    </xsl:attribute>  
    <xsl:choose>
      <xsl:when test="$record_el/@state='Ready'">
        <xsl:attribute name="jsxdevstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_device_ready.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@state='NotReady'">
        <xsl:attribute name="jsxdevstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_device_notready.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@state='Unmanaged'">
        <xsl:attribute name="jsxdevstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_device_unmanaged.png</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxdevstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_device_offline.png</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
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
    <xsl:attribute name="jsxsessionstatus">
      <xsl:value-of select="ConnectionStatus"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="ConnectionStatus=''">
        <xsl:attribute name="jsxsessionstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png</xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="jsxsessionstatus">JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="jsxdevStreamType">
      <xsl:value-of select="$record_el/@streamType"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/@streamType='V2D'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_v2d.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MPEGTS'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_mpgts.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='UDP'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_udp.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_rtp.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-RAW'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_rtpraw.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-PCM'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_rtppcm.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RFB'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_rfb.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='VNC'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_vnc.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MULTI'">
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_multi.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType=''">
        <xsl:attribute name="jsxdevtype"></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <!--HACK. middleware is returning streamtype for PC client as 'V2D MPEGTS RTP' indtead of MULTI -->
        <xsl:attribute name="jsxdevtype">JSXAPPS/IPVS_ClientUI/images/icons/icon_multi.png</xsl:attribute>
        <xsl:attribute name="jsxdevStreamType">MULTI</xsl:attribute>
        <xsl:if test="$record_el/@streamType=''">
          <xsl:attribute name="jsxdevtype"></xsl:attribute>
          <xsl:attribute name="jsxdevStreamType"></xsl:attribute>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="jsxdevTags">
      <xsl:variable name="strTags">
        <xsl:for-each select="Groups/Tag">
          <xsl:text>,</xsl:text><xsl:value-of select="@Name" />=<xsl:value-of select="@Value" />
        </xsl:for-each>
      </xsl:variable>
      <xsl:value-of select="substring-after($strTags, ',')"/>
    </xsl:attribute>
    <xsl:for-each select="ConnectionStatus/Session/Connection">
      <xsl:choose>
        <xsl:when test="($NIDArgument=@srcNID and string(@srcNID)!='')or ($NIDArgument=@dstNID and string(@dstNID)!='') ">
          <xsl:attribute name="jsxselected">1</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>         
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="connection_attributes">
    <xsl:param name="connection_el"/>  
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
  </xsl:template>

</xsl:stylesheet>