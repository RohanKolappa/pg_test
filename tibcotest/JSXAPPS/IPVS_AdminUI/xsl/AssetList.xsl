<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  <xsl:param name="jsximgmssoftwareversion"></xsl:param>
  <xsl:param name="jsximgdevicesoftwareversion"></xsl:param>
  <xsl:template match="/">
          <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="GetDeviceResponse">
    <data jsxid="jsxroot">
      <xsl:apply-templates/>
    </data>
  </xsl:template>
  <xsl:variable name="MSDeviceVersion">
     <xsl:value-of select="$jsximgmssoftwareversion"/>
  </xsl:variable>
  <xsl:variable name="DeviceSoftwareVersion">
    <xsl:value-of select="$jsximgdevicesoftwareversion"/>
  </xsl:variable>
  <xsl:template match="GetDeviceResponse/Device">
    <record>
      <xsl:call-template name="record_attributes">
        <xsl:with-param name="record_el" select="."/>
      </xsl:call-template>
    </record>
  </xsl:template>
  <xsl:template match="GetDeviceResponse/Device[@type='WebClient-Device']">
       <record>
           <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
            <xsl:apply-templates select="DeviceList/Device"></xsl:apply-templates>
       </record>
  </xsl:template>
  <xsl:template match="GetDeviceResponse/Device/DeviceList/Device">
       <record>
          <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
          </xsl:call-template>
       </record>
  </xsl:template>   

  <xsl:template name="record_attributes">
    <xsl:param name="record_el"/>  
    <xsl:attribute name="jsxid">
      <xsl:value-of select="$record_el/@NID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxtext">
      <xsl:value-of select="$record_el/@title"/>
    </xsl:attribute>
    <xsl:attribute name="jsxTemplateAssetResourceNID">
        <xsl:value-of select="$record_el/TemplateNID"/>
    </xsl:attribute>
    <xsl:attribute name="jsxAssetFeature">
        <xsl:value-of select="$record_el/@unManned"/>
    </xsl:attribute>
    <xsl:choose>
        <xsl:when test="$record_el/@type='V2D-Tx-Device'"><xsl:attribute name="assetType">V2D I50 Tx</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='ORB-500-Device'"><xsl:attribute name="assetType">OneRoom(500)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-Rx-Device'"><xsl:attribute name="assetType">V2D I50 Rx</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XP200-Device'"><xsl:attribute name="assetType">V2D(XP200)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XP200S-Device'"><xsl:attribute name="assetType">V2D(XP200S)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XP220-Device'"><xsl:attribute name="assetType">V2D(XP220)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XP220S-Device'"><xsl:attribute name="assetType">V2D(XP220S)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XP100-Device'"><xsl:attribute name="assetType">V2D(XP100)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='V2D-XPi-Device'"><xsl:attribute name="assetType">NGS-D200</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-110-ENCDP-Device'"><xsl:attribute name="assetType">MNA 110 Encoder(DP)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-110-ENCDVI-Device'"><xsl:attribute name="assetType">MNA 110 Encoder(DVI)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-110-ENCHDSDI-Device'"><xsl:attribute name="assetType">MNA 110 Encoder(HDSDI)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-120-ENCDVIAudio-Device'"><xsl:attribute name="assetType">MNA 120 Encoder(DVI + Audio)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-120-ENCANAAudio-Device'"><xsl:attribute name="assetType">MNA 120 Encoder(ANA + Audio)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-120-ENCDPAudio-Device'"><xsl:attribute name="assetType">MNA 120 Encoder(DP + Audio)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-120-ENCHDSDIAudio-Device'"><xsl:attribute name="assetType">MNA 120 Encoder(HDSDI + Audio)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MNA-120-DECDVIAudio-Device'"><xsl:attribute name="assetType">MNA 120 Decoder(DVI + Audio)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-100-Device'"><xsl:attribute name="assetType">DMS(100) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-150-Device'"><xsl:attribute name="assetType">DMS(150) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-200-Device'"><xsl:attribute name="assetType">DMS(200) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1000-Device'"><xsl:attribute name="assetType">DMS(1000) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1200-Device'"><xsl:attribute name="assetType">DMS(1200) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1500-Device'"><xsl:attribute name="assetType">DMS(1500) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1550-Device'"><xsl:attribute name="assetType">DMS(1550) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1600-Device'"><xsl:attribute name="assetType">DMS(1600) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-1700-Device'"><xsl:attribute name="assetType">DMS(1700) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='DMS-25-Device'"><xsl:attribute name="assetType">DMS(25) Media Server</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='SG-100-Device'"><xsl:attribute name="assetType">Streaming Gateway</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MS-25-Device'"><xsl:attribute name="assetType">Management Server(25)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MS-Device'"><xsl:attribute name="assetType">Management Server(100)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MS-150-Device'"><xsl:attribute name="assetType">Management Server(150)</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='Template'"><xsl:attribute name="assetType">Dynamic Client Template</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='IPVS-MgmtClient'"><xsl:attribute name="assetType">Media Client</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='WebClient-Device'"><xsl:attribute name="assetType">Web Client</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='Proxy-Device'"><xsl:attribute name="assetType">Proxy Client</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='IPVSClient-Device' and ($record_el/@unManned='true')"><xsl:attribute name="assetType">Remote Operated Client</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='IPVSClient-Device'"><xsl:attribute name="assetType">Media Client</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MPEGTS-Tx-Device'"><xsl:attribute name="assetType">MPEGTS(Tx) Encoder</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='MPEGTS-Rx-Device'"><xsl:attribute name="assetType">MPEGTS(Rx) Decoder</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='RTP-Tx-Device'"><xsl:attribute name="assetType">RTP(Tx) Encoder</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='RTP-Rx-Device'"><xsl:attribute name="assetType">RTP(Rx) Decoder</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='UDP-Tx-Device'"><xsl:attribute name="assetType">UDP(Tx) Encoder</xsl:attribute></xsl:when>
        <xsl:when test="$record_el/@type='UDP-Rx-Device'"><xsl:attribute name="assetType">UDP(Rx) Decoder</xsl:attribute></xsl:when>
        <xsl:otherwise><xsl:attribute name="assetType"> </xsl:attribute></xsl:otherwise>
     </xsl:choose>
    <xsl:attribute name="assetTypeOrginal">
        <xsl:value-of select="$record_el/@type"/>
    </xsl:attribute>
    <xsl:choose>
        <xsl:when test="$record_el/@state='Ready' and ($record_el/Status[@device='FAILED'])">
          <xsl:attribute name="assetStatus">FAILED</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NeedsUpgrade' and ($record_el/Status[@device='FAILED'])">
          <xsl:attribute name="assetStatus">FAILED</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NeedsUpgrade'">
          <xsl:attribute name="assetStatus">UPGRADE</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Unmanaged'">
          <xsl:attribute name="assetStatus">Unknown</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="assetStatus"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="assetStatusOrginal">
        <xsl:value-of select="$record_el/@state"/>
    </xsl:attribute>
    <xsl:choose>
        <xsl:when test="$record_el/@state='Ready' and ($record_el/Status[@device='OK'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_greenBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Ready' and ($record_el/Status[@device='ERRORS'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_orangeBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Ready' and ($record_el/Status[@device='FAILED'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_redBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Ready' and ($record_el/@type='WebClient-Device'or $record_el/@type='Proxy-Device')">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_greenBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NeedsUpgrade' and ($record_el/Status[@device='OK'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_greenball.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NeedsUpgrade' and ($record_el/Status[@device='ERRORS'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_orangeBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NeedsUpgrade' and ($record_el/Status[@device='FAILED'])">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_redBall.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='NotReady'">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_redball.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Offline'">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_grayball.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Template'">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_grayball.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="$record_el/@state='Unmanaged'">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_grayball.png</xsl:attribute>
        </xsl:when>
        <!--If device is auto registered then Status[@device] is empty so check type="ANY" and presence="available"-->
        <xsl:when test="$record_el/@type='ANY' and ($record_el/@presence='available')">
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_grayball.png</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="jsximgDeviceStatus">images/icons/icon_whiteBall.png</xsl:attribute>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
        <xsl:when test="$record_el/@msStatus='Active'">
          <xsl:attribute name="assetMSStatus">images/icons/icon_standalone_active.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='Primary')">
          <xsl:attribute name="assetMSStatus">images/icons/icon_primary_active.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='PrimaryStandby')">
          <xsl:attribute name="assetMSStatus">images/icons/icon_primary_standby.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='Secondary')">
          <xsl:attribute name="assetMSStatus">images/icons/icon_secondary_active.png</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='SecondaryStandby')">
          <xsl:attribute name="assetMSStatus">images/icons/icon_secondary_standby.png</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="assetMSStatus"> </xsl:attribute>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='Active')">
          <xsl:attribute name="assetMSStatusOriginal">standalone</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='Primary')">
          <xsl:attribute name="assetMSStatusOriginal">primary-active</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='PrimaryStandby')">
          <xsl:attribute name="assetMSStatusOriginal">primary-standby</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='Secondary')">
          <xsl:attribute name="assetMSStatusOriginal">secondary-active</xsl:attribute>
        </xsl:when>
        <xsl:when test="($record_el/@type='MS-Device' or $record_el/@type='MS-150-Device') and ($record_el/@msStatus='SecondaryStandby')">
          <xsl:attribute name="assetMSStatusOriginal">secondary-standby</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="assetMSStatusOriginal"> </xsl:attribute>
        </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="assetDescription">
        <xsl:value-of select="$record_el/@description"/>
    </xsl:attribute>
    <xsl:attribute name="assetContactOwnerJID">
        <xsl:value-of select="$record_el/@ownerUserJID"/>
    </xsl:attribute>
    <xsl:attribute name="deviceHAStatus"> </xsl:attribute>
    <xsl:attribute name="deviceCPU"> </xsl:attribute>
    <xsl:attribute name="deviceUptime"> </xsl:attribute>
    <xsl:attribute name="deviceMemory"> </xsl:attribute>
    <xsl:attribute name="deviceDisk"> </xsl:attribute>
    <xsl:variable name="deviceVersion"><xsl:value-of select="$record_el/@softwareVersion"/></xsl:variable>
    <xsl:choose>
      <xsl:when test="$record_el/@type !='V2D-XPi-Device' and  $MSDeviceVersion=$deviceVersion">
        <xsl:attribute name="deviceSWVersion"><xsl:value-of select="$record_el/@softwareVersion"/></xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@type='V2D-XPi-Device' and ($DeviceSoftwareVersion='' or $DeviceSoftwareVersion=$deviceVersion)">
        <xsl:attribute name="deviceSWVersion"><xsl:value-of select="$record_el/@softwareVersion"/></xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@state='Unmanaged' or $record_el/@state='Template' or $record_el/@type='Proxy-Device' or $record_el/@type='IPVSClient-Device'">
        <xsl:attribute name="deviceSWVersion"> </xsl:attribute>
      </xsl:when>
      <xsl:when test="$record_el/@softwareVersion=''">
        <xsl:attribute name="deviceSWVersion"> </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="deviceSWVersion">&lt;span class='swUpgrade'&gt;<xsl:value-of select="$record_el/@softwareVersion"/>&lt;/span&gt;</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:attribute name="deviceFWVersion"> </xsl:attribute>
    <xsl:attribute name="deviceNTPClient"> </xsl:attribute>
    <xsl:attribute name="deviceConnection"> </xsl:attribute>
    <xsl:choose>
      <xsl:when test="$record_el/PortList">
       <xsl:attribute name="devicePortNIDs">
         <xsl:variable name="portNIDValues">
             <xsl:for-each select="$record_el/PortList/Port">
                <xsl:sort select="@deviceElementID"/>
                <xsl:value-of select="concat(@NID, ',')"/>
             </xsl:for-each>
         </xsl:variable>
         <xsl:value-of select="$portNIDValues"/>
       </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="$record_el/Groups">
        <xsl:for-each select="$record_el/Groups/Tag">
          <xsl:sort select="@Name"/>
          <xsl:variable name="tagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
          <xsl:variable name="tagValue"><xsl:value-of select="@Value"/></xsl:variable>
          <xsl:if test="$tagName!=''">
            <xsl:variable name="tagAA"><xsl:value-of select="concat('jsxTagValue',$tagName)"/></xsl:variable>
            <!--
            <xsl:if test="position()=1 or (position()>1 and following-sibling::Tag/@Name != $tagName) or(position()=last() and preceding-sibling::Tag/@Name = $tagName) or (position()=last() and preceding-sibling::Tag/@Name != $tagName)">
            -->
            <xsl:if test="position()=1 or (position()>1) ">
              <xsl:attribute name="{$tagAA}">
                <xsl:variable name="ConcatedTagValues">
                  <xsl:for-each select="$record_el/Groups/Tag">
                    <xsl:sort select="@Name"/>
                    <xsl:variable name="sortTagName"><xsl:value-of select="translate(@Name,' ','')"/></xsl:variable>
                    <xsl:if test="$tagName=$sortTagName">
                      <xsl:value-of select="concat(',',@Value)"/>
                    </xsl:if>
                  </xsl:for-each>
                </xsl:variable>
                <xsl:value-of select="substring-after($ConcatedTagValues, ',')"/>
              </xsl:attribute>
            </xsl:if>
          </xsl:if>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
      </xsl:otherwise>
    </xsl:choose>  
  </xsl:template>
</xsl:stylesheet>