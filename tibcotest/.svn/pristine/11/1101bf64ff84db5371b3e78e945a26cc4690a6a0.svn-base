<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8" />
  <xsl:template match="/">
        <data jsxid="jsxroot">
      <xsl:apply-templates />
    </data>
  </xsl:template>

  <xsl:template match="/GetSession/Session/EntityList/Entity[@type='DstPort']">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="." />
        <xsl:with-param name="root" select="/GetSession" />
      </xsl:call-template>
    </record>
  </xsl:template>

  
  <xsl:template name="record_attributes">
	<xsl:param name="record_el" />
	<xsl:param name="root" />
	<xsl:variable name="xpath_entity2"></xsl:variable>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@id" />
    </xsl:attribute>    
    <xsl:attribute name="jsxSessionDestPresence">
      <xsl:value-of select="$record_el/@presence" />
    </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$record_el/@presence='available'">
              <xsl:attribute name="jsxSessionDestPresence">images/icons/icon_greenBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@presence=''">
              <xsl:attribute name="jsxSessionDestPresence">images/icons/icon_lightgrayBall.png</xsl:attribute>
        </xsl:when>            
        <xsl:when test="$record_el/@presence='unavailable'">
              <xsl:attribute name="jsxSessionDestPresence">images/icons/icon_grayBall.png</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
            <xsl:attribute name="jsxSessionDestPresence">images/icons/icon_grayBall.png</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    <xsl:attribute name="jsxSessionDestName">
      <xsl:value-of select="$record_el/@name" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionDestType">
      <xsl:value-of select="$record_el/@streamType" />
    </xsl:attribute>
	  <xsl:choose>
      <xsl:when test="$record_el/@streamType='V2D'">
            <xsl:attribute name="jsxSessionDestType">images/icons/icon_v2d.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MPEGTS'">
                  <xsl:attribute name="jsxSessionDestType">images/icons/icon_mpgts.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='UDP'">
                  <xsl:attribute name="jsxSessionDestType">images/icons/icon_udp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP'">
                  <xsl:attribute name="jsxSessionDestType">images/icons/icon_rtp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-RAW'">
        <xsl:attribute name="jsxSessionDestType">images/icons/icon_rtpraw.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-PCM'">
        <xsl:attribute name="jsxSessionDestType">images/icons/icon_rtppcm.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RFB'">
        <xsl:attribute name="jsxSessionDestType">images/icons/icon_rfb.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='VNC'">
          <xsl:attribute name="jsxSessionDestType">images/icons/icon_vnc.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MULTI'">
                  <xsl:attribute name="jsxSessionDestType">images/icons/icon_multi.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType=''">
     <xsl:attribute name="jsxSessionDestType"></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <!--HACK. middleware is returning streamtype for PC client as 'V2D MPEGTS RTP' indtead of MULTI -->
         <xsl:attribute name="jsxSessionDestType">images/icons/icon_multi.png</xsl:attribute>
         <xsl:if test="$record_el/@streamType=''">
             <xsl:attribute name="jsxSessionDestType"></xsl:attribute>
         </xsl:if>
      </xsl:otherwise>
    </xsl:choose> 
    <xsl:attribute name="jsxSessionDestOverlay">
    </xsl:attribute>
    <xsl:attribute name="jsxSessionDestLUT">
    </xsl:attribute>
    <xsl:attribute name="jsxSessionDestFrameRate">
    </xsl:attribute>
  </xsl:template>
</xsl:stylesheet>