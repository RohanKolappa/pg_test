<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="/">
		<CLIConfigs>
			<xsl:for-each select="document($getrequestfile)//Group">
			<xsl:variable name="thispos" select="position()"/>
				<CLIConfig>
				<Command><Config><xsl:value-of select="./ObjectEntry/@name" /></Config>
				<CLICommand><xsl:value-of select="document($setrequestfile)//CLI[position()=$thispos]"/></CLICommand>
				 <Operation>Modify</Operation>
    			 <Source>CLI</Source>				
				<Status><xsl:value-of select="./Response/Attribute[@name='Status']/@value"/></Status>
				<FailureReason><xsl:value-of select="./Response/Attribute[@name='Reason']/@value"/></FailureReason>
				<ClientData></ClientData>		
				
				</Command>
				<Parameters>
				<xsl:for-each select="./ObjectEntry/Attribute">
					<Parameter>
					<Name><xsl:value-of select="@name"/></Name>
					<Value><xsl:value-of select="@value"/></Value>
					</Parameter>
				</xsl:for-each>
				
				</Parameters>
				</CLIConfig>
			</xsl:for-each>
		</CLIConfigs>
	</xsl:template>	
</xsl:stylesheet>
                		
