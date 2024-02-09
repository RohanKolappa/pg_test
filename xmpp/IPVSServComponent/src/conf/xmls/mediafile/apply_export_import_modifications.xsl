<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
    <xsl:template match="@* | node()">
    <xsl:apply-templates mode="before" select="." />
    <xsl:copy>
        <xsl:apply-templates select="@*" />
        <xsl:apply-templates mode="add-atts" select="." />
        <xsl:apply-templates mode="insert" select="." />
        <xsl:apply-templates />
        <xsl:apply-templates mode="append" select="." />
    </xsl:copy>
    <xsl:apply-templates mode="after" select="." />
    </xsl:template>
    <!-- By default, don't add anything -->
    <xsl:template mode="add-atts" match="*" />
    <xsl:template mode="insert" match="*" />
    <xsl:template mode="append" match="*" />
    <xsl:template mode="before" match="@* | node()" />
    <xsl:template mode="after" match="@* | node()" />

	<!-- server params -->
    <xsl:param name="parentMediaDirNID"></xsl:param>
    <xsl:param name="UUID"></xsl:param>
    <xsl:param name="mediaStreamFileResourceNID"></xsl:param>
    <xsl:param name="mediaGroupID"></xsl:param>
    <xsl:param name="NID"></xsl:param>
	<xsl:param name="dateModified" />
	<xsl:param name="mediaStoreFile_Rev" />
	<xsl:param name="owner" />
    
    <xsl:param name="state_flag">Busy</xsl:param>
    <xsl:param name="state_msg">Exporting</xsl:param>

	<!--  common for all  -->    
    <xsl:template match="@NID">
        <xsl:attribute name="NID"><xsl:value-of select="$NID"/></xsl:attribute>
    </xsl:template>
       
    <!--  mediaclip / mediagroup updates  -->
    <xsl:template match="/MediaStreamFileResource/Info/MediaStoreFileInfo/MediaGroupID">
		<xsl:choose>
			<xsl:when test="string-length(normalize-space(.))>0">
				 <MediaGroupID><xsl:attribute name="ClipID"><xsl:value-of select="@ClipID" /></xsl:attribute><xsl:value-of select="$mediaGroupID" /></MediaGroupID>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy-of select="." />
			</xsl:otherwise>
		</xsl:choose>
    </xsl:template>
    <xsl:template match="/MediaStreamFileResource/Info/Properties/Owner">
    	<Owner><xsl:value-of select="$owner"/></Owner>
    </xsl:template>    
    <xsl:template match="/MediaStreamFileResource/Info/Properties/ParentMediaDirNID">
   		<xsl:choose>
            <xsl:when test="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type='MediaClip' or Info/MediaStoreFileInfo/Type='PVRMediaClip'] or string-length(normalize-space(.)) > 0">
            	<ParentMediaDirNID><xsl:value-of select="$parentMediaDirNID"/></ParentMediaDirNID> 
            </xsl:when>
            <xsl:otherwise>
                <ParentMediaDirNID></ParentMediaDirNID>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>    
	 
    <!--  only mediaclip updates  -->
    <xsl:template match="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type!='MediaGroup']/State/StateFlag">
    	<StateFlag><xsl:value-of select="$state_flag"/></StateFlag>
    </xsl:template>
    <xsl:template match="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type!='MediaGroup']/State/StateMsg">
    	<StateMsg><xsl:value-of select="$state_msg"/></StateMsg>
    </xsl:template>
    <xsl:template match="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type!='MediaGroup']/Info/UUID">
    	<UUID><xsl:value-of select="$UUID"/></UUID>
    </xsl:template>    
    <xsl:template match="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type!='MediaGroup']//MediaStoreFileInfo/@MediaStoreFile-Rev">
        <xsl:attribute name="MediaStoreFile-Rev"><xsl:value-of select="$mediaStoreFile_Rev"/></xsl:attribute>
    </xsl:template>
    <xsl:template match="/MediaStreamFileResource[Info/MediaStoreFileInfo/Type!='MediaGroup']/Info/Properties/Permissions/Write">
    	<Write>false</Write>
    </xsl:template>
    
    <!-- 
    <xsl:template match="MediaStreamFileResource[Info/MediaStoreFileInfo/Type='MediaClip']/Info/Properties/DateModified">
        <DateModified><xsl:value-of select="$dateModified" /></DateModified>
    </xsl:template>   
      -->       
    <!--  only bookmarks updates  -->
     <xsl:template match="Bookmark/MediaStreamFileResourceNID">
    	<MediaStreamFileResourceNID><xsl:value-of select="$mediaStreamFileResourceNID"/></MediaStreamFileResourceNID>
    </xsl:template>    
    
    <!--  clear out FFTracks if streamType is V2D  -->
    <xsl:template match="/MediaStreamFileResource[Info/StreamTypeList/StreamType='V2D']/Info//MediaStreamProfileInfo/V2DMediaStreamProfileInfo/Info/V2DRecorderConfig/FFTracks">
    	<FFTracks></FFTracks>
    </xsl:template> 
    
</xsl:stylesheet>