<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	
	<xsl:param name="filepath"></xsl:param>
	<xsl:param name="UUID"></xsl:param>
    
    <xsl:variable name="dbXML">
		  <xsl:value-of select="$filepath"/>
    </xsl:variable>
	
	<xsl:template match="text()" />

	<xsl:template match="index">
		<MediaInfo>
			<Title>
				<xsl:value-of select="$UUID" />
			</Title>
			<Description>
				<xsl:value-of select="//record[@id='MediaFile']/Type" />
			</Description>
			<Type>
				<xsl:value-of select="//record[@id='MediaFile']/Type" />
			</Type>
			<DateCreated>
				<xsl:value-of select="//record[@id='MediaFile']/DateCreated" />
			</DateCreated>
			<Duration>
			</Duration>

			<xsl:copy-of select="document($dbXML)//MetaData" />
			
			<BookmarkList/>
			
		</MediaInfo>
	</xsl:template>

</xsl:stylesheet>