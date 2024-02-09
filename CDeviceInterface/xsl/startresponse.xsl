<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>

    <xsl:template match="//@from">
        <xsl:attribute name="from">
			<xsl:value-of select="$jid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@userJID">
        <xsl:attribute name="userJID">
			<xsl:value-of select="$tojid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@streamNID">
        <xsl:attribute name="streamNID">
			<xsl:value-of select="$streamnid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//@requestNID">
        <xsl:attribute name="requestNID">
			<xsl:value-of select="$requestnid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>


    <xsl:template match="//@roomNID">
        <xsl:attribute name="roomNID">
			<xsl:value-of select="$roomnid"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>


    <xsl:template match="//Last/@WC">
        <xsl:attribute name="WC">
			<xsl:value-of select="$lastwc"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//ClientData">
		<xsl:copy-of select="document($requestfile)//ClientData"/>
    </xsl:template>

    <xsl:template match="//ClientCallback">
		<xsl:copy-of select="document($requestfile)//ClientCallback"/>
    </xsl:template>


    <xsl:template match="//Last/TC">
        <xsl:attribute name="TC">
			<xsl:value-of select="$lasttc"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//Start/@TC">
        <xsl:attribute name="TC">
			<xsl:value-of select="$starttc"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

    <xsl:template match="//Start/@WC">
        <xsl:attribute name="WC">
			<xsl:value-of select="$startwc"></xsl:value-of>
		</xsl:attribute>
    </xsl:template>

     <xsl:template match="//@to">
		<xsl:attribute name="to">
			<xsl:copy-of select="$tojid"/>
        </xsl:attribute>
    </xsl:template>


    <xsl:template match="//URL">
		<xsl:copy>
			<xsl:value-of select="$url"></xsl:value-of>
		</xsl:copy>
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

