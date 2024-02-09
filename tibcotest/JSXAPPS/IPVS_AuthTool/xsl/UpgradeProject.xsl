<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
      <PROJECT>
        <NAME><xsl:value-of select="PROJECT/NAME"/></NAME>
        <CREATED><xsl:value-of select="PROJECT/CREATED"/></CREATED>
        <LENGTH><xsl:value-of select="PROJECT/LENGTH"/></LENGTH>
        <SIZE><xsl:value-of select="PROJECT/SIZE"/></SIZE>
        <OUTPUTTYPE>IPVSMEDIA</OUTPUTTYPE>
        <OUTPUTPATH><xsl:value-of select="PROJECT/DVDPATH"/></OUTPUTPATH>
        <IPVSMEDIA>
          <LOGOPATH><xsl:value-of select="PROJECT/LOGOPATH"/></LOGOPATH>
          <LOGOTITLE><xsl:value-of select="PROJECT/LOGOTITLE"/></LOGOTITLE>
          <DVDDATE><xsl:value-of select="PROJECT/DVDDATE"/></DVDDATE>
        </IPVSMEDIA>
        <TRANSCODEDMEDIA>
          <CONFIGURATION name="">
            <VIDEO type="" mode="" bitrate="" quant="" frameRate="" resolution=""></VIDEO>
            <AUDIO type="" bitrate=""></AUDIO>
            <FORMAT type=""></FORMAT>
          </CONFIGURATION>
        </TRANSCODEDMEDIA>
        <MEDIALIST>
          <xsl:apply-templates select="PROJECT/MEDIALIST"></xsl:apply-templates>
        </MEDIALIST>
      </PROJECT>
    </xsl:template>

    <xsl:template match="PROJECT/MEDIALIST/MEDIA">
      <MEDIA>
        <xsl:call-template name="translate_clip">
          <xsl:with-param name="clip_el" select="."/>
          <xsl:with-param name="is_Root" select="''"/>
        </xsl:call-template>
        <xsl:apply-templates select="CLIP"></xsl:apply-templates>
      </MEDIA>
    </xsl:template>

    <xsl:template match="PROJECT/MEDIALIST/MEDIA/CLIP">
      <CLIP>
        <xsl:call-template name="translate_clip">
          <xsl:with-param name="clip_el" select="."/>
          <xsl:with-param name="is_Root" select="'false'"/>
        </xsl:call-template>
      </CLIP>
    </xsl:template>
  
    <xsl:template match="PROJECT/MEDIALIST/CLIP">
	     <CLIP>
         <xsl:call-template name="translate_clip">
           <xsl:with-param name="clip_el" select="."/>
           <xsl:with-param name="is_Root" select="''"/>
         </xsl:call-template>
	     </CLIP>
    </xsl:template>
  
    <xsl:template name="translate_clip">
      <xsl:param name="clip_el"/>
      <xsl:param name="is_Root"/>
      <xsl:attribute name="ID"><xsl:value-of select="$clip_el/FILENAME"/></xsl:attribute>
      <xsl:attribute name="isActive">true</xsl:attribute>
      <xsl:attribute name="isRoot"><xsl:value-of select="$is_Root"/></xsl:attribute>
      <TITLE><xsl:value-of select="$clip_el/TITLE"/></TITLE>
      <TYPE><xsl:value-of select="$clip_el/TYPE"/></TYPE>
      <DATETIME><xsl:value-of select="$clip_el/DATETIME"/></DATETIME>
      <xsl:choose>
        <xsl:when test="$clip_el/CLIPLENGTH">
          <LENGTH><xsl:value-of select="$clip_el/CLIPLENGTH"/></LENGTH>
          <PLAYLENGTH><xsl:value-of select="$clip_el/CLIPLENGTH"/></PLAYLENGTH>
        </xsl:when>
        <xsl:otherwise>
          <LENGTH><xsl:value-of select="$clip_el/LENGTH"/></LENGTH>
          <PLAYLENGTH><xsl:value-of select="$clip_el/LENGTH"/></PLAYLENGTH>
        </xsl:otherwise>
      </xsl:choose>
      
      <SIZE><xsl:value-of select="$clip_el/SIZE"/></SIZE>
      <BASEPATH><xsl:value-of select="$clip_el/BASEPATH"/></BASEPATH>
      <FILENAME><xsl:value-of select="$clip_el/FILENAME"/></FILENAME>
      <SDPINFO><xsl:value-of select="$clip_el/SDPINFO"/></SDPINFO>
      <ISTWODOTXMEDIA><xsl:value-of select="$clip_el/ISTWODOTXMEDIA"/></ISTWODOTXMEDIA>
      <SEGMENTLIST></SEGMENTLIST>
    </xsl:template>
  </xsl:stylesheet>

           
