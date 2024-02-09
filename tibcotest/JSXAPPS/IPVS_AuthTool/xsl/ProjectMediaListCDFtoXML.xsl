<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
	      <MEDIALIST>
           <xsl:apply-templates/>
	      </MEDIALIST>
    </xsl:template>

    <xsl:template match="data/record">
       <xsl:call-template name="translate_data_record">
           <xsl:with-param name="record_el" select="."/>
       </xsl:call-template>
    </xsl:template> 

    <xsl:template name="translate_data_record">
      <xsl:param name="record_el"/>
      <xsl:choose>
          <xsl:when test="$record_el/@jsxGrpNID='NONE'">
            <CLIP>
               <xsl:call-template name="translate_record">
                  <xsl:with-param name="record_el" select="."/>
               </xsl:call-template>
            </CLIP>
	        </xsl:when>
	        <xsl:otherwise>
	           <MEDIA>
                <xsl:call-template name="translate_group_record">
                  <xsl:with-param name="record_el" select="."/>
                </xsl:call-template>
	              <xsl:apply-templates select="record"></xsl:apply-templates>
             </MEDIA>
	        </xsl:otherwise>
      </xsl:choose> 
    </xsl:template>

     <xsl:template match="data/record/record">
        <CLIP>
          <xsl:call-template name="translate_record">
	          <xsl:with-param name="record_el" select="."/>
          </xsl:call-template>
        </CLIP>
    </xsl:template>

    <xsl:template name="translate_record">
       <xsl:param name="record_el"/>
       <xsl:attribute name="ID"><xsl:value-of select="$record_el/@jsxid"/></xsl:attribute>
       <xsl:attribute name="isActive"><xsl:value-of select="$record_el/@jsxActive"/></xsl:attribute>
       <xsl:attribute name="isRoot"><xsl:value-of select="$record_el/@isRoot"/></xsl:attribute>
       <TITLE><xsl:value-of select="$record_el/@jsxName"/></TITLE>
       <TYPE><xsl:value-of select="$record_el/@jsxStreamType"/></TYPE>
       <DATETIME><xsl:value-of select="$record_el/@jsxCreatedDate"/></DATETIME>
       <LENGTH><xsl:value-of select="$record_el/@jsxLength"/></LENGTH>
       <PLAYLENGTH><xsl:value-of select="$record_el/@jsxPlayLength"/></PLAYLENGTH>
       <SIZE><xsl:value-of select="$record_el/@jsxSize"/></SIZE>
       <BASEPATH><xsl:value-of select="$record_el/@jsxBasePath"/></BASEPATH>
       <FILENAME><xsl:value-of select="$record_el/@jsxFileNID"/></FILENAME>
       <SDPINFO><xsl:value-of select="$record_el/@jsxSDPInfo"/></SDPINFO>
       <ISTWODOTXMEDIA><xsl:value-of select="$record_el/@jsxIsTwodotXMedia"/></ISTWODOTXMEDIA>
       <SEGMENTLIST><xsl:copy-of select="$record_el/SEGMENTLIST/*"/></SEGMENTLIST> 
    </xsl:template>

   <xsl:template name="translate_group_record">
      <xsl:param name="record_el"/>
      <xsl:attribute name="ID"><xsl:value-of select="$record_el/@jsxid"/></xsl:attribute>
      <xsl:attribute name="isRoot"><xsl:value-of select="$record_el/@isRoot"/></xsl:attribute>
      <TITLE><xsl:value-of select="$record_el/@jsxName"/></TITLE>
      <TYPE><xsl:value-of select="$record_el/@jsxStreamType"/></TYPE>
      <DATETIME><xsl:value-of select="$record_el/@jsxCreatedDate"/></DATETIME>
      <LENGTH><xsl:value-of select="$record_el/@jsxLength"/></LENGTH>
      <PLAYLENGTH><xsl:value-of select="$record_el/@jsxPlayLength"/></PLAYLENGTH>
      <SIZE><xsl:value-of select="$record_el/@jsxSize"/></SIZE>
      <BASEPATH><xsl:value-of select="$record_el/@jsxBasePath"/></BASEPATH>
      <FILENAME><xsl:value-of select="$record_el/@jsxFileNID"/></FILENAME>
      <SDPINFO><xsl:value-of select="@jsxSDPInfo"/></SDPINFO>
      <ISTWODOTXMEDIA><xsl:value-of select="$record_el/@jsxIsTwodotXMedia"/></ISTWODOTXMEDIA>
      <SEGMENTLIST><xsl:copy-of select="$record_el/SEGMENTLIST/*"/></SEGMENTLIST> 
    </xsl:template>
  </xsl:stylesheet>

           
