<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template match="node()|@*">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()" />
		</xsl:copy>
	</xsl:template>
	<xsl:param name="path"></xsl:param>
	 <xsl:param name="dateCreated" />
	<xsl:variable name="Metadata">
		<xsl:value-of select="$path" />
	</xsl:variable>
	<xsl:template match="MediaStoreFile/MetaData">
		<xsl:copy-of select="document($Metadata)//MetaData" />
	</xsl:template>
	<xsl:template match="MediaStoreFile/Title">
		<Title>
			<xsl:value-of select="document($Metadata)//@title" />
		</Title>
	</xsl:template>
	<xsl:template match="MediaStoreFile/Description">
		<xsl:copy-of select="document($Metadata)//Description" />
	</xsl:template>
	<xsl:template match="//DateCreated">
      <DateCreated><xsl:value-of select="$dateCreated"/></DateCreated>
    </xsl:template>
</xsl:stylesheet>