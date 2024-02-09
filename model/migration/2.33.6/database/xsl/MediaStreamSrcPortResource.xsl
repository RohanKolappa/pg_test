<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.33.4</xsl:attribute>
	</xsl:template>
	
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.5</xsl:attribute>
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
	
	<xsl:template match="//ReverseTCPConnMode">
		<xsl:choose>
			<xsl:when test=".='true'">
				<ConnMode>Pull</ConnMode>
			</xsl:when>
			<xsl:otherwise>
				<ConnMode>Push</ConnMode>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>	
	
	
	<xsl:template mode="append" match="//MediaStreamProfileInfo">
	  <WebRTCMediaStreamProfileInfo>
		  <WebRTCStreamConfig>
		  </WebRTCStreamConfig>
	  </WebRTCMediaStreamProfileInfo>
	</xsl:template>	
	
</xsl:stylesheet>
