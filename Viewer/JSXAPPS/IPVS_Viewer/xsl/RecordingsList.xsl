<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
	<data jsxid="jsxroot">
            <xsl:apply-templates/>
	 </data>
    </xsl:template>

    <xsl:template match="GetFile">
	<xsl:attribute name="startFrom"><xsl:value-of select="@from"/></xsl:attribute>
	<xsl:attribute name="objectCount"><xsl:value-of select="@total"/></xsl:attribute>	
         <xsl:apply-templates/>
    </xsl:template>
   
    <xsl:template match="GetFile/File[@fileType='MediaClip' or @fileType='MediaImage']">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template> 
       </record>
    </xsl:template>    

  <xsl:template match="GetFile/File[@fileType='MediaGroup']">
    <xsl:apply-templates select="FileList/File"></xsl:apply-templates>
  </xsl:template>

    <xsl:template match="GetFile/File/FileList/File">
       <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
       </record>
    </xsl:template>

   <xsl:template name="record_attributes">
          <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>

           <xsl:attribute name="jsxbypassAutoStorageRules"><xsl:value-of select="$record_el/@bypassAutoStorageRules"/></xsl:attribute>
           <xsl:choose>
             <xsl:when test="$record_el/@fileType='MediaImage'">
               <xsl:choose>
                 <xsl:when test="$record_el/@bypassAutoStorageRules='true'">
                   <xsl:attribute name="jsximg">images/icons/icon_camera_lock.png</xsl:attribute>
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:attribute name="jsximg">images/icons/icon_camera.png</xsl:attribute>
                 </xsl:otherwise>
               </xsl:choose>
             </xsl:when>
             <xsl:when test="$record_el/@fileType='MediaClip'">
               <xsl:choose>
                 <xsl:when test="$record_el/@bypassAutoStorageRules='true'">
                   <xsl:attribute name="jsximg">images/icons/icon_reel_lock.png</xsl:attribute>
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:attribute name="jsximg">images/icons/icon_reel.png</xsl:attribute>
                 </xsl:otherwise>
               </xsl:choose>
             </xsl:when>
             <xsl:when test="$record_el/@fileType='MediaGroup'">
               <xsl:choose>
                 <xsl:when test="$record_el/@bypassAutoStorageRules='true'">
                   <xsl:attribute name="jsximg">images/icons/icon_group_lock.png</xsl:attribute>
                 </xsl:when>
                 <xsl:otherwise>
                   <xsl:attribute name="jsximg">images/icons/icon_group.png</xsl:attribute>
                 </xsl:otherwise>
               </xsl:choose>
             </xsl:when>
             <xsl:otherwise>
               <xsl:attribute name="jsximg"> </xsl:attribute>
             </xsl:otherwise>
           </xsl:choose>

          <xsl:attribute name="jsxfileType"><xsl:value-of select="$record_el/@fileType"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaName"><xsl:value-of select="$record_el/@title"/></xsl:attribute>
          <xsl:attribute name="jsxmediagroupID"><xsl:value-of select="$record_el/@mediaGroupID"/></xsl:attribute>
	        <xsl:attribute name="jsxmediastate"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
          <xsl:attribute name="jsxmediaURL"><xsl:value-of select="$record_el/@mediaURL"/></xsl:attribute>
          <xsl:choose>
            <xsl:when test="$record_el/@state='Ready'">
                  <xsl:attribute name="jsxtextmediaStatus">images/icons/icon_device_ready.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='NotReady'">
                  <xsl:attribute name="jsxtextmediaStatus">images/icons/icon_device_notready.png</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="jsxtextmediaStatus">images/icons/icon_device_offline.png</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
         <xsl:choose>
           <xsl:when test="number($record_el/@size)">          
            <xsl:attribute name="jsxtextmediaSize"><xsl:value-of select='round($record_el/@size div 1048576)'/></xsl:attribute>
           </xsl:when>
           <xsl:otherwise>
             <xsl:attribute name="jsxtextmediaSize">0</xsl:attribute>
           </xsl:otherwise>
         </xsl:choose>
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
          <xsl:variable name="t3" select="concat($t31,($sec mod 60),':')"/>
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
          <xsl:variable name="t2" select="concat($t21,($min mod 60),':')"/>
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
          <xsl:attribute name="jsxtextmediaLength">
           <xsl:value-of select="concat(concat($t1,$t2),concat($t3,$t4))"/>
          </xsl:attribute>
          <xsl:attribute name="jsxtextmediaCreated"><xsl:value-of select="$record_el/@dateCreated"/></xsl:attribute>
	        <xsl:attribute name="jsxtextPatient"><xsl:value-of select="$record_el/MetaData/Tag[@Name='Patient']"/></xsl:attribute>
          <xsl:attribute name="jsxtextmediaStreamType"><xsl:value-of select="$record_el/@streamType"/></xsl:attribute>
          <xsl:attribute name="jsxclipID"><xsl:value-of select="$record_el/@clipID"/></xsl:attribute>
          <xsl:attribute name="jsxsessionConfigNID"><xsl:value-of select="$record_el/@sessionConfigNID"/></xsl:attribute>
	  <xsl:copy-of select="MetaData"/>
    </xsl:template>


    </xsl:stylesheet>
