<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes" />

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.29.14</xsl:attribute>
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

        <xsl:template match="LastRunStatus" />

	<xsl:template match="FTPStatus">
	    <LastExportStatus>
		<xsl:attribute name="date">
		    <xsl:value-of select="@date" />
		</xsl:attribute>
		<xsl:attribute name="time">
             	    <xsl:value-of select="@time" />
		</xsl:attribute>
		<xsl:attribute name="state">
  		    <xsl:value-of select="@state" />
		</xsl:attribute>
	    </LastExportStatus>
	    <LastExported>
	      <xsl:choose>
		<xsl:when test="@state='SUCCESS'">
		    <xsl:attribute name="date">
			<xsl:value-of select="@date" />
		    </xsl:attribute>
		    <xsl:attribute name="time">
			<xsl:value-of select="@time" />
		    </xsl:attribute>
   		</xsl:when>
		<xsl:otherwise>
		    <xsl:attribute name="date"></xsl:attribute>
		    <xsl:attribute name="time"></xsl:attribute>
		</xsl:otherwise>
	      </xsl:choose>
	    </LastExported>
	</xsl:template>

</xsl:stylesheet>
