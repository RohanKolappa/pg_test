<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>    
    <xsl:template mode="insert" match="//StreamInfoList">
	<xsl:element name="StreamInfo">
				<xsl:attribute name="streamNID">
					     <xsl:value-of select="$streamnid"></xsl:value-of>
				</xsl:attribute>
			    	<xsl:element name="StreamDataDoc">
						<xsl:copy-of select="document($streamrequestfile)//data"/>					
				</xsl:element>
				<xsl:element name="MediaSourceDataDoc">
						<xsl:copy-of select="document($mediarequestfile)//data"/>
				</xsl:element>
			</xsl:element>
    </xsl:template>

    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
    <xsl:template match="@* | node()">
        <xsl:apply-templates mode="before" select="."/>
        <xsl:copy>
            <xsl:apply-templates select="@*"/>
            <xsl:apply-templates mode="add-atts" select="."/>
            <xsl:apply-templates mode="insert" select="."/>
            <xsl:apply-templates/>
            <xsl:apply-templates mode="append" select="."/>
        </xsl:copy>
        <xsl:apply-templates mode="after" select="."/>
    </xsl:template>

    <!-- By default, don't add anything -->
    <xsl:template mode="add-atts" match="*"/>
    <xsl:template mode="insert" match="*"/>
    <xsl:template mode="append" match="*"/>
    <xsl:template mode="before" match="@* | node()"/>
    <xsl:template mode="after" match="@* | node()"/>
</xsl:stylesheet>

