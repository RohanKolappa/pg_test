<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.32.1</xsl:attribute>
	</xsl:template>
	
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">2.5</xsl:attribute>
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

      <!-- DELETE RecorderConfig ELEMENTS  -->
      
    <xsl:template match="//V2DMediaStreamProfileInfo/V2DRecorderConfig"/>
    <xsl:template match="//RTPMediaStreamProfileInfo/RTPRecoderConfig"/>
    <xsl:template match="//RTP-RAWMediaStreamProfileInfo/RTPRAWRecoderConfig"/>
    <xsl:template match="//RTP-PCMMediaStreamProfileInfo/RTPPCMRecoderConfig"/>
    <xsl:template match="//RFBMediaStreamProfileInfo/RFBRecoderConfig"/>

	
</xsl:stylesheet>

 
 
