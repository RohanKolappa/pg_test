<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.29.16</xsl:attribute>
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

	<xsl:template match="//PVRInfo" />
	<xsl:template match="//MediaStreamList" />

	<xsl:template match="//ProfileInfo" mode="after">
	   <PVRInfo>
		<xsl:attribute name="enable" />
		<xsl:attribute name="trimLength" />
		<xsl:attribute name="mediaStreamFileResourceNID" />
		<xsl:attribute name="mediaStreamDirResourceNID" />
		<xsl:attribute name="sessionId" />
	   </PVRInfo>
	</xsl:template>

	<xsl:template match="//MaxInstanceCount" mode="after">
	   <MediaStreamList>
	      <MediaStream>	
		<xsl:attribute name="NID" />
		<xsl:attribute name="id" />
		<xsl:attribute name="role" />
		<xsl:attribute name="roomType" />
		<xsl:attribute name="roomNID" />
		<xsl:attribute name="roomID" />
              </MediaStream>
	   </MediaStreamList>
	</xsl:template>

	<xsl:template match="//ResourceInstance">
	    <Instance>
		<xsl:attribute name="id" /> 
		<xsl:apply-templates select="@*|node()" />
	    </Instance>
	</xsl:template>

	<xsl:template match="//MediaStreamProfileInfo" mode="append">
	    <DVIMediaStreamProfileInfo>
                 <DVIStreamConfig>
                      <IsDVII></IsDVII>
                 </DVIStreamConfig>
            </DVIMediaStreamProfileInfo>
	</xsl:template>
	
</xsl:stylesheet>

 
 
