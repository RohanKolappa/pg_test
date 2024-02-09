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
    <xsl:param name="type">MediaClip</xsl:param>
    <xsl:param name="filePath"></xsl:param>
    <xsl:param name="playlength"></xsl:param>
    <xsl:param name="dateCreated"></xsl:param>
    
    <xsl:variable name="MediaFile">
		<xsl:value-of select="$filePath"/>
    </xsl:variable>
    
    <xsl:template match="/">
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="MediaStoreFile/Type[.='PVRMediaClip']">
		<Type><xsl:value-of select="$type"/></Type>
	</xsl:template>
	
	<xsl:template match="MediaStoreFile/PVRMediaClip">
		<MediaClip><xsl:apply-templates select="@*|node()" /></MediaClip>
	</xsl:template>
	
	<xsl:template match="MediaStoreFile/DateCreated">
		<DateCreated><xsl:value-of select="$dateCreated"/></DateCreated>
	</xsl:template>
	<xsl:template match="//PlayLength">
		<PlayLength><xsl:value-of select="$playlength"/></PlayLength>
	</xsl:template>
	
	<xsl:template match="TrimLength" />
	<xsl:template match="//FFTracks">
		<FFTracks />
	</xsl:template>
</xsl:stylesheet>