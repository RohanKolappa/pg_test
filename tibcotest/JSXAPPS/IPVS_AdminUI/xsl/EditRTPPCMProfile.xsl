﻿<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="RTP-PCMMediaStreamProfile">
        <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>
        <xsl:attribute name="serviceVer">
            <xsl:value-of select="$record_el/@serviceVer"/>
        </xsl:attribute>      
        <xsl:attribute name="Ver">
            <xsl:value-of select="$record_el/@Ver"/>
        </xsl:attribute>   
        <xsl:attribute name="Rev">
            <xsl:value-of select="$record_el/@Rev"/>
        </xsl:attribute> 
        <xsl:attribute name="parentNID">
            <xsl:value-of select="$record_el/@parentNID"/>
        </xsl:attribute>
        <xsl:attribute name="StreamType">
            <xsl:value-of select="$record_el/StreamType"/>
        </xsl:attribute> 
        <xsl:attribute name="Priority">
            <xsl:value-of select="$record_el/Priority"/>
        </xsl:attribute> 
        <xsl:attribute name="Title">
            <xsl:value-of select="$record_el/Title"/>
        </xsl:attribute> 
        <xsl:attribute name="Description">
            <xsl:value-of select="$record_el/Description"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowPause='true'">
                <xsl:attribute name="AllowPause">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowPause">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowSkipback='true'">
                <xsl:attribute name="AllowSkipback">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowSkipback">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:choose>
          <xsl:when test="$record_el/StreamActionInfo/AllowSkipfwd='true'">
                <xsl:attribute name="AllowSkipfwd">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="AllowSkipfwd">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="PCMSDP-Template-Src">
            <xsl:value-of select="$record_el/Info/SDP-Template-Src"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/Info/RFBPCMStreamConfig/IsMulticast='true'">
                <xsl:attribute name="RTPPCMIsMulticast">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="RTPPCMIsMulticast">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose> 
        <xsl:attribute name="PCMSDP-m-Template">
            <xsl:value-of select="$record_el/Info/RFBPCMStreamConfig/SDP-m-Template"/>
        </xsl:attribute>
        <xsl:attribute name="RTPPCMAVOption">
            <xsl:value-of select="$record_el/Info/RFBPCMStreamConfig/AVOption"/>
        </xsl:attribute>
        <xsl:attribute name="PCMThumbnailEnable">
            <xsl:value-of select="$record_el/Info/RTPPCMRecoderConfig/ThumbnailConfig/Enable"/>
        </xsl:attribute>
     </xsl:template>
</xsl:stylesheet>