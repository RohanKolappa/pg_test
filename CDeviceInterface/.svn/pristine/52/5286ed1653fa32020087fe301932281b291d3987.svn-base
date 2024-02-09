<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>


    <xsl:template match="//state">
        <xsl:element name="state">
                <xsl:value-of select="$state"></xsl:value-of>
         </xsl:element>
     </xsl:template>
     <xsl:template match="//@relayRoomNID">
        <xsl:attribute name="relayRoomNID">
                <xsl:value-of select="$relayroomnid"></xsl:value-of>
         </xsl:attribute>
     </xsl:template>
      <xsl:template match="//@relayStreamNID">
        <xsl:attribute name="relayStreamNID">
                <xsl:value-of select="$relaystreamnid"></xsl:value-of>
         </xsl:attribute>
     </xsl:template>
     <xsl:template match="//lastEventData">
	<xsl:copy-of select="document($requestfile)//Data/*"/>	
     </xsl:template>
     <xsl:template match="//StreamStatusEvent/Exception">    	  		
     </xsl:template> 	
		

    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
    <xsl:template match="@* | node()" name="copy">
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

