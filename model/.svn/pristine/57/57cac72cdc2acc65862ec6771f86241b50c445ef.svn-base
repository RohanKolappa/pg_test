<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" indent="no" />
	<xsl:template match="/">
		<xsl:for-each select="//Log">			
			<xsl:value-of select="@date" />
			<xsl:text>, </xsl:text>
			<xsl:value-of select="@time" />
			<xsl:text>, </xsl:text>	
			<xsl:value-of select="@type" />
			<xsl:text>, </xsl:text>
			<xsl:value-of select="@name" />			
			<xsl:text>, </xsl:text>
			<xsl:value-of select="@status" />			
			<xsl:text>, </xsl:text>				
			<xsl:value-of select="@user" />
			<xsl:text>, </xsl:text>
			<xsl:value-of select="@resource" />			
			<xsl:text>, </xsl:text>		
			<xsl:value-of select="@target" />			
			<xsl:text>, </xsl:text>		
			
			<xsl:choose>
				<xsl:when test="contains(./Description/@text,'ERROR')">          
					<xsl:value-of select="substring-before(./Description/@text, 'ERROR:')" />
					<xsl:text>, </xsl:text>
			    	<xsl:value-of select="substring-after(./Description/@text, 'ERROR:')" />			    	
			  	</xsl:when>
				<xsl:otherwise>
    				<xsl:value-of select="./Description/@text" />
  				</xsl:otherwise>
			</xsl:choose>	  				
			<xsl:text>&#xa;</xsl:text>
		</xsl:for-each>
		<xsl:text> </xsl:text>
	</xsl:template>
</xsl:stylesheet>