<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.26.1</xsl:attribute>
	</xsl:template>
	<xsl:template match="@Ver">
        <xsl:attribute name="Ver">1.7</xsl:attribute>
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
	
	<xsl:template match="//XAuthInfo">
	   <xsl:element name="ObserverInfo">
	      <xsl:element name="Observer">
	         <xsl:attribute name="xAuth"> 
	          <xsl:value-of select="ForceXAuth"/>
	         </xsl:attribute>
	         <xsl:attribute name="monitor"> 
	          <xsl:value-of select="ForceXAuth"/>
	         </xsl:attribute>
	         <xsl:element name="UserNID"/>
	         <xsl:element name="ResourceNID">
	           <xsl:value-of select="XAuthOperatorList/XAuthResourceNID"/>
	         </xsl:element>
	      </xsl:element>
	   </xsl:element>	   
	</xsl:template>		


	<xsl:template match="//Enable444">
		<xsl:element name="ColorSampling"/>
	</xsl:template>
	
	<xsl:template match="//V2DEncoderConnectionParameterConfig" mode="append">
		<xsl:element name="IFrameInterval"/>
	</xsl:template>
    
</xsl:stylesheet>
 
 