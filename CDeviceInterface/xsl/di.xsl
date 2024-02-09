<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>

    <xsl:template match="//@title">
        <xsl:attribute name="title">
			<xsl:value-of select="$title"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@password">
        <xsl:attribute name="password">
			<xsl:value-of select="$password"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@state">
        <xsl:attribute name="state">
			<xsl:value-of select="$state"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@type">
        <xsl:attribute name="type">
			<xsl:value-of select="$type"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//DeviceConfig">
		<xsl:copy-of select="document($configfile)//DeviceConfig"/>
    </xsl:template>

    <xsl:template match="//EthernetPortStatus/ParametersList">
		<xsl:copy-of select="document($networkstatusfile)//ParametersList"/>
    </xsl:template>

    <xsl:template match="//XMPPAgentServiceStatus/Parameters">
		<xsl:copy-of select="document($xmppstatusfile)//Parameters"/>
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

