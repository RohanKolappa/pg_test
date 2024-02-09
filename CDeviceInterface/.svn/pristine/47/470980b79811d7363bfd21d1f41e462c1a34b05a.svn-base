<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>
    <xsl:variable name="responseDataname">
		<xsl:value-of select="$responsename"/>
    </xsl:variable>
   <xsl:variable name="responseInnername">
                <xsl:value-of select="$innerresponsename"/>
    </xsl:variable>
    <xsl:template match="//@from">
        <xsl:attribute name="from">
			<xsl:value-of select="$tojid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>
    <xsl:template match="//@requestNID">
        <xsl:attribute name="requestNID">
                        <xsl:value-of select="document($requestfile)//@requestNID"></xsl:value-of>
                </xsl:attribute>
    </xsl:template>
    <xsl:template match="//@requestName">
        <xsl:attribute name="requestName">
                        <xsl:value-of select="document($requestfile)//@requestName"></xsl:value-of>
                </xsl:attribute>
    </xsl:template>

    <xsl:template match="//@userJID">
        <xsl:attribute name="userJID">
			<xsl:value-of select="$tojid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>


    <xsl:template match="//ClientData">
		<xsl:copy-of select="document($requestfile)//ClientData"/>
    </xsl:template>

    <xsl:template match="//ClientCallback">
		<xsl:copy-of select="document($requestfile)//ClientCallback"/>
    </xsl:template>


    <xsl:template match="//Data">
	<xsl:element name="Data">
        <xsl:element name="{$responseDataname}">
		<xsl:element name="{$responseInnername}">
                </xsl:element>
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

