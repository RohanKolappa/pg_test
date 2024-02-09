<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.30.2</xsl:attribute>
	</xsl:template>
	
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">2.1</xsl:attribute>
    </xsl:template>
	
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

	<xsl:template mode="append" match="//V2DMediaStreamProfileInfo/V2DRecorderConfig">
		<xsl:choose>
			<xsl:when test="ThumbnailConfig">
			</xsl:when>
			<xsl:otherwise>
				<ThumbnailConfig>
                	<Enable>false</Enable>
                </ThumbnailConfig>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template mode="append" match="//RTPMediaStreamProfileInfo">
		<xsl:choose>
			<xsl:when test="RTPRecoderConfig">
			</xsl:when>
			<xsl:otherwise>
				<RTPRecoderConfig>
               		<ThumbnailConfig>
                  		<Enable>false</Enable>
               		</ThumbnailConfig>
            	</RTPRecoderConfig>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
</xsl:stylesheet>

 
 
