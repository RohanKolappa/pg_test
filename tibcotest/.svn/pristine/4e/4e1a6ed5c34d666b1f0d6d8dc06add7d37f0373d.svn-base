<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8" />
  <xsl:template match="/">
    <data jsxid="jsxroot">
      <xsl:apply-templates />
    </data>
  </xsl:template>

  <xsl:template match="/GetSession/Session/EntityList/Entity[@type='RecFile' or @type='PVRFile']">
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
    <xsl:attribute name="jsxSessionPVRPresence">
      <xsl:value-of select="$record_el/@presence" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionPVRName">
      <xsl:value-of select="$record_el/@name" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionPVRStreamType">
      <xsl:value-of select="$record_el/@streamType" />
    </xsl:attribute>
	  <xsl:choose>
      <xsl:when test="$record_el/@streamType='V2D'">
            <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_v2d.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MPEGTS'">
                  <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_mpgts.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='UDP'">
                  <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_udp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP'">
                  <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_rtp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-RAW'">
        <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_rtpraw.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-PCM'">
        <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_rtppcm.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RFB'">
        <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_rfb.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='VNC'">
            <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_vnc.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MULTI'">
                  <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_multi.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType=''">
     <xsl:attribute name="jsxSessionPVRStreamType"></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <!--HACK. middleware is returning streamtype for PC client as 'V2D MPEGTS RTP' indtead of MULTI -->
         <xsl:attribute name="jsxSessionPVRStreamType">images/icons/icon_multi.png</xsl:attribute>
         <xsl:attribute name="jsxdevStreamType">MULTI</xsl:attribute> 
         <xsl:if test="$record_el/@streamType=''">
             <xsl:attribute name="jsxSessionPVRStreamType"></xsl:attribute>
         </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="jsxSessionPVRType">
      <xsl:value-of select="$record_el/@type" />
    </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$record_el/@type='PVRFile'">
              <xsl:attribute name="jsxSessionPVRType">PVR</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@type='RecFile'">
              <xsl:attribute name="jsxSessionPVRType">Recording</xsl:attribute>
        </xsl:when>            
        <xsl:when test="$record_el/@type=''">
              <xsl:attribute name="jsxSessionPVRType"></xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
            <xsl:attribute name="jsxSessionPVRType"></xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>


    <xsl:attribute name="jsxSessionPVRDir">
    </xsl:attribute>
    <xsl:attribute name="jsxSessionSourceLUT">
    </xsl:attribute>
    <xsl:attribute name="jsxSessionSourceFrameRate">
    </xsl:attribute>
  </xsl:template>
</xsl:stylesheet>