<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8" />
  <xsl:template match="/">
    <data jsxid="jsxroot">
      <xsl:apply-templates />
    </data>
  </xsl:template>

  <xsl:template match="/GetSession/Session/ConnectionList/Connection">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="." />
        <xsl:with-param name="root" select="/GetSession" />
      </xsl:call-template>
      <xsl:apply-templates select="ConnSeg"></xsl:apply-templates>
    </record>
  </xsl:template>

  <xsl:template match="GetSession/Session/ConnectionList/Connection/ConnSeg">
    <record>
     <xsl:attribute name="jsxid">
        <xsl:value-of select="@srcResourceNID" />
     </xsl:attribute>
     <xsl:attribute name="jsxSessionSourceType">
        <xsl:value-of select="../@type" />
     </xsl:attribute>
     <xsl:attribute name="jsxSessionConnectionSource">
        <xsl:value-of select="@srcResourceTitle" />
     </xsl:attribute>
     <xsl:attribute name="jsxSessionConnectionDest">
        <xsl:value-of select="@dstResourceTitle" />
     </xsl:attribute>
     <xsl:attribute name="jsxSessionConnectionStatus">
        <xsl:value-of select="@state" />
     </xsl:attribute>
      <xsl:choose>
        <xsl:when test="@state='SETUP_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="@state='START_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="@state='SOURCE_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="@state='MEDIA_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="@state='DEST_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>        
        <xsl:when test="@state='error'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="jsxSessionConnectionErrorCode"></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"></xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>     
     <xsl:attribute name="jsxSessionConnectionStreamType">
        <xsl:value-of select="../@streamType" />
     </xsl:attribute>
	  <xsl:choose>
      <xsl:when test="../@streamType='V2D'">
            <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_v2d.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="../@streamType='MPEGTS'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_mpgts.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="../@streamType='UDP'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_udp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="../@streamType='RTP'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="../@streamType='RTP-RAW'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtpraw.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="../@streamType='RTP-PCM'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtppcm.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="../@streamType='RFB'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rfb.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="../@streamType='VNC'">
          <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_vnc.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="../@streamType='MULTI'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_multi.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="../@streamType=''">
     <xsl:attribute name="jsxSessionConnectionStreamType"></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <!--HACK. middleware is returning streamtype for PC client as 'V2D MPEGTS RTP' indtead of MULTI -->
         <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_multi.png</xsl:attribute>
         <xsl:if test="../@streamType=''">
             <xsl:attribute name="jsxSessionConnectionStreamType"></xsl:attribute>
         </xsl:if>
      </xsl:otherwise>
    </xsl:choose> 
     <xsl:attribute name="jsxSessionConnectionProfile">
        <xsl:value-of select="@profileTitle" />
     </xsl:attribute>
     <xsl:attribute name="jsxSessionConnectionMultiCastIP">
        <xsl:value-of select="@multicastIP" />
     </xsl:attribute>
     <xsl:attribute name="jsxDestResourceNID">
        <xsl:value-of select="@dstResourceNID" />
     </xsl:attribute>
     <xsl:attribute name="jsxProfileNID">
        <xsl:value-of select="@profileNID" />
     </xsl:attribute>
     <xsl:attribute name="jsxSrcResourceNID">
        <xsl:value-of select="@srcResourceNID" />
     </xsl:attribute>
    </record>
  </xsl:template>
  
  <xsl:template name="record_attributes">
	<xsl:param name="record_el" />
	<xsl:param name="root" />
	<xsl:variable name="xpath_entity2"></xsl:variable>
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@id" />
    </xsl:attribute>    
    <xsl:attribute name="jsxSessionSourceType">
      <xsl:value-of select="$record_el/@type" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionConnectionSource">
      <xsl:value-of select="$root/Session/EntityList/Entity[@id=$record_el/@srcEntityId]/@name" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionConnectionDest">
      <xsl:value-of select="$root/Session/EntityList/Entity[@id=$record_el/@dstEntityId]/@name" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionConnectionStatus">
      <xsl:value-of select="$record_el/@state" />
    </xsl:attribute>
      <xsl:choose>
        <xsl:when test="$record_el/@state='SETUP_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='START_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='SOURCE_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='MEDIA_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='DEST_ERROR'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>        
        <xsl:when test="$record_el/@state='error'">
          <xsl:attribute name="jsxSessionConnectionErrorCode"><xsl:value-of select="$record_el/@errorCode" /></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"><xsl:value-of select="$record_el/@errorData" /></xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="jsxSessionConnectionErrorCode"></xsl:attribute>
          <xsl:attribute name="jsxSessionConnectionErrorDesc"></xsl:attribute>
        </xsl:otherwise>
      </xsl:choose> 
    <xsl:attribute name="jsxSessionConnectionStreamType">
      <xsl:value-of select="$record_el/@streamType" />
    </xsl:attribute>
	  <xsl:choose>
      <xsl:when test="$record_el/@streamType='V2D'">
            <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_v2d.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MPEGTS'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_mpgts.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='UDP'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_udp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtp.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-RAW'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtpraw.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RTP-PCM'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rtppcm.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='RFB'">
        <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_rfb.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='VNC'">
          <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_vnc.png</xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@streamType='MULTI'">
                  <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_multi.png</xsl:attribute>
            </xsl:when>
      <xsl:when test="$record_el/@streamType=''">
     <xsl:attribute name="jsxSessionConnectionStreamType"></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <!--HACK. middleware is returning streamtype for PC client as 'V2D MPEGTS RTP' indtead of MULTI -->
         <xsl:attribute name="jsxSessionConnectionStreamType">images/icons/icon_multi.png</xsl:attribute>
         <xsl:if test="$record_el/@streamType=''">
             <xsl:attribute name="jsxSessionConnectionStreamType"></xsl:attribute>
         </xsl:if>
      </xsl:otherwise>
    </xsl:choose> 
    <xsl:attribute name="jsxSessionConnectionProfile">
      <xsl:value-of select="$record_el/ConnSeg/@profileTitle" />
    </xsl:attribute>
    <xsl:attribute name="jsxSessionConnectionMultiCastIP">
      <xsl:value-of select="$record_el/ConnSeg/@multicastIP" />
    </xsl:attribute>
  </xsl:template>
</xsl:stylesheet>