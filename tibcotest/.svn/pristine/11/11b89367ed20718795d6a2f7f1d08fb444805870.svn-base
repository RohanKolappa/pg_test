<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
     <xsl:template match="/">
            <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="GetDirResponse">
       <data jsxid="jsxroot">
          <xsl:apply-templates/>
       </data>
    </xsl:template>   

    <xsl:template match="GetDirResponse/Dir[@title!='PVR' and @title!='RECYCLEBIN']">
       <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
            <xsl:copy-of select="./Groups" />
            <xsl:copy-of select="./SubDirInfo" />
            <xsl:copy-of select="./Permissions" />
            <xsl:copy-of select="./StorageConfig/DirSpanning/PortInfo" />
       </record>
    </xsl:template>

    <xsl:template name="record_attributes">
          <xsl:param name="record_el"/>
          <xsl:attribute name="jsxid"><xsl:value-of select="$record_el/@NID"/></xsl:attribute>
          <xsl:attribute name="jsxDirName"><xsl:value-of select="$record_el/@title"/></xsl:attribute>   
          <xsl:attribute name="jsxTotalSize"><xsl:value-of select="$record_el/@size"/></xsl:attribute>
          <xsl:attribute name="jsxSizeUsed"><xsl:value-of select="$record_el/@sizeUsed"/></xsl:attribute>
          <xsl:attribute name="jsxFullThreshold"><xsl:value-of select="$record_el/StorageConfig/DirSpanning/@diskFullThreshold"/></xsl:attribute>
          <xsl:attribute name="jsxReadOnly"><xsl:value-of select="$record_el/@readOnly"/></xsl:attribute>

          <xsl:attribute name="jsxStateFlag"><xsl:value-of select="$record_el/@state"/></xsl:attribute>
          <xsl:choose>
            <xsl:when test="$record_el/@state='Ready'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_ready.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='NotReady'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_notready.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Offline'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_offline.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Busy'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_Busy.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Extracting'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_extracting.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Updating'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_updating.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Gettingremotepath'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_getting_remote.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Transferring'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_transferring.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Detached'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_unregistered.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='StartSynch'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_start_synch.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Error'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_failure.png</xsl:attribute>
            </xsl:when>
            <xsl:when test="$record_el/@state='Deleting'">
              <xsl:attribute name="jsxStatus">images/icons/icon_device_deleting.png</xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="jsxStatus">images/icons/icon_device_failure.png</xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="$record_el/SubDirInfo/Dir[@title='PVR']">
              <xsl:attribute name="jsxPVRDirNID"><xsl:value-of select="$record_el/SubDirInfo/Dir[@title='PVR']/@NID"/></xsl:attribute>
              <xsl:attribute name="jsxPVRMode">ENABLED</xsl:attribute> 
            </xsl:when>
            <xsl:otherwise>
                        <xsl:attribute name="jsxPVRMode">DISABLED</xsl:attribute> 
            </xsl:otherwise>
          </xsl:choose>

          <xsl:choose>
            <xsl:when test="$record_el/SubDirInfo/Dir[@title='RECYCLEBIN']">
              <xsl:attribute name="jsxRecycleBinDirNID"><xsl:value-of select="$record_el/SubDirInfo/Dir[@title='RECYCLEBIN']/@NID"/></xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
            </xsl:otherwise>
          </xsl:choose>

    </xsl:template>

</xsl:stylesheet>