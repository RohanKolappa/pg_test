<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="GetPVRFile">
       <data jsxid="jsxroot">
            <xsl:apply-templates/>
       </data>
    </xsl:template> 
   
    <xsl:template match="GetPVRFile/File[@fileType='PVRMediaClip']">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
          <xsl:copy-of select="./Permissions" />
       </record>
    </xsl:template>    

    <xsl:template match="GetPVRFile/File[@fileType='MediaGroup']">
       <record>
           <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
            <xsl:copy-of select="./Permissions" />
            <xsl:apply-templates select="FileList/File"></xsl:apply-templates>
       </record>
    </xsl:template>

    <xsl:template match="GetPVRFile/File/FileList/File">
       <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
            <xsl:copy-of select="./Permissions" />
       </record>
    </xsl:template>

   <xsl:template name="record_attributes">
          <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaPVRName"><xsl:value-of select="$record_el/@title"/></xsl:attribute>
          <xsl:attribute name="jsxmediaGroupID"><xsl:value-of select="$record_el/@mediaGroupID"/></xsl:attribute>
	        <xsl:attribute name="jsxtextmediaStateFlag"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaStatus"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaPVRSize"><xsl:value-of select="$record_el/@size"/></xsl:attribute>
          <xsl:attribute name="jsxPvrPlayLength"><xsl:value-of select="$record_el/@playlength"/></xsl:attribute>

     <xsl:choose>
       <xsl:when test="$record_el/@fileType='MediaImage'"><xsl:attribute name="jsxMediaFileImg">../../addins/ipvsCommon/images/icons/icon_camera.png</xsl:attribute></xsl:when>
       <xsl:when test="$record_el/@fileType='PVRMediaClip'"><xsl:attribute name="jsxMediaFileImg">../../addins/ipvsCommon/images/icons/icon_reel.png</xsl:attribute></xsl:when>
       <xsl:when test="$record_el/@fileType='MediaGroup'"><xsl:attribute name="jsxMediaFileImg">../../addins/ipvsCommon/images/icons/icon_group.png</xsl:attribute></xsl:when>
       <xsl:otherwise><xsl:attribute name="jsxMediaFileImg"> </xsl:attribute></xsl:otherwise>
     </xsl:choose>
         <xsl:choose>
         <xsl:when test="number($record_el/@playlength) > 0">
           <xsl:variable name="ms" select="$record_el/@playlength" />
           <xsl:variable name="sec" select="floor($ms div 1000)" />
           <xsl:variable name="min" select="floor($sec div 60)"/>
           <xsl:variable name="hr" select="floor($min div 60)"/>
           <xsl:variable name="t41">
             <xsl:choose>
               <xsl:when test="($ms mod 1000)> 99">
                 <xsl:value-of select="''"/>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:value-of select="'0'"/>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <xsl:variable name="t42">
             <xsl:choose>
               <xsl:when test="($ms mod 1000)> 9">
                 <xsl:value-of select="''"/>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:value-of select="'0'"/>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <xsl:variable name="t4" select="concat($t41,concat($t42,($ms mod 1000)))"/>
           <xsl:variable name="t31">
             <xsl:choose>
               <xsl:when test="($sec mod 60)> 9">
                 <xsl:value-of select="''"/>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:value-of select="'0'"/>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <xsl:variable name="t3" select="concat($t31,($sec mod 60))"/>
           <xsl:variable name="t21">
             <xsl:choose>
               <xsl:when test="($min mod 60)> 9">
                 <xsl:value-of select="''"/>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:value-of select="'0'"/>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <xsl:variable name="t2" select="concat($t21,($min mod 60),'')"/>
           <xsl:variable name="t11">
             <xsl:choose>
               <xsl:when test="($hr mod 60)> 9">
                 <xsl:value-of select="''"/>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:value-of select="'0'"/>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:variable>
           <xsl:variable name="t1" select="concat($t11,($hr mod 60),':')"/>
           <xsl:attribute name="jsxtextmediaPVRAvail">
             <xsl:value-of select="concat($t1,$t2)"/>
           </xsl:attribute>
         </xsl:when>
         <xsl:otherwise>
           <xsl:attribute name="jsxtextmediaPVRAvail">00:00</xsl:attribute>
         </xsl:otherwise>
         </xsl:choose>



          <xsl:attribute name="jsxtextmediaPVRCreated"><xsl:value-of select="$record_el/@dateCreated"/></xsl:attribute>
          <xsl:attribute name="jsxclipID"><xsl:value-of select="$record_el/@clipID"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaReadOnly"><xsl:value-of select="$record_el/@readOnly"/></xsl:attribute>
          <xsl:choose>
            <xsl:when test="$record_el/Groups">
              <xsl:for-each select="$record_el/Groups/Tag">
                <xsl:variable name="tagName"><xsl:value-of select="@Name" /></xsl:variable>
                <xsl:variable name="tagValue"><xsl:value-of select="@Value" /></xsl:variable>
                <xsl:choose>
                  <xsl:when test="$tagName!=''">
                    <xsl:variable name="tagAA"><xsl:value-of select="concat('jsxTagValue',$tagName)" /></xsl:variable>
                    <xsl:attribute name="{$tagAA}"><xsl:value-of select="$tagValue"/></xsl:attribute>
                  </xsl:when>
                  <xsl:otherwise>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
            </xsl:otherwise>
          </xsl:choose>

          <xsl:choose>
            <xsl:when test="$record_el/MetaData">
              <xsl:for-each select="$record_el/MetaData/Tag">
                <xsl:variable name="metaName"><xsl:value-of select="translate(@Name,' ','')" /></xsl:variable>
                <xsl:variable name="metaValue"><xsl:value-of select="." /></xsl:variable>
                <xsl:choose>
                  <xsl:when test="$metaName!=''">
                    <xsl:variable name="metaAA"><xsl:value-of select="concat('jsxMetaData',$metaName)" /></xsl:variable>
                    <xsl:attribute name="{$metaAA}"><xsl:value-of select="$metaValue"/></xsl:attribute>
                  </xsl:when>
                  <xsl:otherwise>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
            </xsl:otherwise>
          </xsl:choose>
   </xsl:template>


    </xsl:stylesheet>
