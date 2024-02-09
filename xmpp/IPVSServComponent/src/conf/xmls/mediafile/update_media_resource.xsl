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
    <xsl:param name="type"></xsl:param>
    <xsl:param name="objectFilePath"></xsl:param>
    <xsl:param name="playLength">0</xsl:param>
   
   <xsl:variable name="MediaFile">
		<xsl:value-of select="$objectFilePath"/>
	</xsl:variable>
	
    <xsl:template match="/">
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="/MediaStreamFileResource/Info/MediaStoreFileInfo/Type">
		<Type><xsl:value-of select="$type"/></Type>
	</xsl:template>
	
	<xsl:template match="/MediaStreamFileResource/Info/PVRMediaClip">
		<MediaClip> 
			<xsl:copy-of select="document($MediaFile)//MediaClip/@*"/>
			<PlayLength><xsl:value-of select="/MediaStreamFileResource/Info/PVRMediaClip/PlayLength"/></PlayLength> 
   			<xsl:copy-of select="/MediaStreamFileResource/Info/PVRMediaClip/MediaStreamProfileInfo"/> 
   			<xsl:copy-of select="document($MediaFile)//MediaClip/*[not(self::PlayLength) and not(self::MediaStreamProfileInfo)]"/>
		</MediaClip> 
	</xsl:template>
	
	<xsl:template match="/MediaStreamFileResource/Info/Properties/ParentMediaDirNID">
		<ParentMediaDirNID><xsl:value-of select="$parentMediaDirNID"/></ParentMediaDirNID>
	</xsl:template>
	
	
</xsl:stylesheet>