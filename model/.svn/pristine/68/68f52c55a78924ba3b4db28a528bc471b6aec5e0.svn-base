<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="rootname"/>
  <xsl:param name="banner_str"/>
	<xsl:output method="xml" version="1.0" encoding="ISO-8859-1" indent="yes" />
		<xsl:template match="/">
			<xsl:text disable-output-escaping="yes">&lt;xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"&gt;</xsl:text>
			<xsl:text>&#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;xsl:template match="/"&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;xsl:text disable-output-escaping="yes"&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;lt;xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"&amp;gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
				<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 

				<xsl:text disable-output-escaping="yes">&lt;xsl:text disable-output-escaping="yes"&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;lt;xsl:output method="xml" version="1.0" encoding="ISO-8859-1" indent="yes" /&amp;gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
				<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
				<xsl:text disable-output-escaping="yes">&lt;xsl:text disable-output-escaping="yes"&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;lt;xsl:template match="@*|node()"&amp;gt; &amp;lt;xsl:copy&amp;gt; &amp;lt;xsl:apply-templates select="@*|node()" /&amp;gt; &amp;lt;/xsl:copy&amp;gt; &amp;lt;/xsl:template&amp;gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 

				<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 

		<xsl:for-each select="//handler[action='MODIFY' or action='ADD' ][device=$rootname]"> 
				<xsl:variable name="operation" select="current()/action"/>
					<xsl:for-each select="paramtable/paramentry">
						<xsl:text>&#10;</xsl:text>
						<xsl:text>&#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;xsl:for-each  select="</xsl:text>
						<xsl:text disable-output-escaping="yes">//Value[../Name='</xsl:text> 
						<xsl:value-of select="paramname"/>
						<xsl:text disable-output-escaping="yes">'][../../../Command/CLICommand='</xsl:text> 
						<xsl:value-of select="../../cmd"/>
						<xsl:text disable-output-escaping="yes">']"&gt;</xsl:text> 
						<xsl:text>&#10;</xsl:text>

						<xsl:text disable-output-escaping="yes">    &lt;xsl:text&gt;</xsl:text>
						<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 

						<xsl:text>&#10;</xsl:text>
						<!-- Start Template -->
						<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
						<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
						<xsl:text>&lt;xsl:template </xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>

						<xsl:text disable-output-escaping="yes">    &lt;xsl:text&gt;match="</xsl:text>
						<xsl:value-of select="../../triggername"/>
						<xsl:choose>
							<xsl:when test='"ADD"=$operation'>
								<xsl:text disable-output-escaping="yes">[@ListIndex=</xsl:text> 
								<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
								<xsl:text>&#10;</xsl:text>
								<!-- Index -->

								<xsl:text disable-output-escaping="yes">    &lt;xsl:value-of select=</xsl:text>
								<xsl:text disable-output-escaping="yes">"../../../Command/Index"&#47;&gt;</xsl:text>
								<xsl:text>&#10;</xsl:text>

								<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
								<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
								<xsl:text>]/</xsl:text>
								<xsl:value-of select="paramval"/>
								
							</xsl:when>
							<xsl:otherwise>
								<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
								<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
								<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
								<xsl:text>/</xsl:text>
								<xsl:value-of select="paramval"/>
								
							</xsl:otherwise>
						</xsl:choose>
						<xsl:text>"&gt;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>

						<xsl:text disable-output-escaping="yes">    &lt;xsl:text&gt;</xsl:text>
						<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>

						<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
						<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
						
						<xsl:variable name="paramvalue" select="current()/paramval"/>
						<xsl:choose>
							<xsl:when test="contains($paramvalue, '/')">
								<xsl:text>&lt;</xsl:text>
								<xsl:value-of select="substring-after($paramvalue,'/')"/>
								<xsl:text>&gt;</xsl:text>
							</xsl:when>
							<xsl:otherwise>
								<xsl:text>&lt;</xsl:text>
								<xsl:value-of select="$paramvalue"/>
								<xsl:text>&gt;</xsl:text>
							</xsl:otherwise>
						</xsl:choose>
						
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>
						<!-- Insert Value -->
						<xsl:variable name="Type" select="@type" />
						<xsl:choose>
							<xsl:when test='"Boolean"=$Type'>
								<xsl:text disable-output-escaping="yes">    &lt;xsl:value-of select="concat(.,'_BOOL_')"/&gt;</xsl:text>
							</xsl:when>
							<xsl:when test='"URLString"=$Type'>
								<xsl:text disable-output-escaping="yes">    &lt;xsl:text&gt;</xsl:text>
								<xsl:value-of select="$banner_str"/>
								<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text>
							</xsl:when>
							<xsl:otherwise>
								<xsl:text disable-output-escaping="yes">    &lt;xsl:value-of select="."/&gt;</xsl:text>
							</xsl:otherwise>
						</xsl:choose>
						<xsl:text>&#10;</xsl:text>

						<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
						<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
						
						<xsl:choose>
							<xsl:when test="contains($paramvalue, '/')">
								<xsl:text>&lt;/</xsl:text>
								<xsl:value-of select="substring-after($paramvalue,'/')"/>
								<xsl:text>&gt;</xsl:text>
							</xsl:when>
							<xsl:otherwise>
								<xsl:text>&lt;/</xsl:text>
								<xsl:value-of select="$paramvalue"/>
								<xsl:text>&gt;</xsl:text>
							</xsl:otherwise>
						</xsl:choose>
            
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>


						<xsl:text disable-output-escaping="yes">    &lt;xsl:text&gt;</xsl:text>
						<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>

						<!-- End Template -->
						<xsl:text disable-output-escaping="yes">    &lt;xsl:text</xsl:text>
						<xsl:text disable-output-escaping="yes"> disable-output-escaping="yes"&gt;</xsl:text>
						<xsl:text>&lt;/xsl:template&gt;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:for-each&gt;</xsl:text>
						<xsl:text>&#10;</xsl:text>
					</xsl:for-each>
					<xsl:text>&#10;</xsl:text>
				</xsl:for-each>
				<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
			<xsl:text>&#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;xsl:text disable-output-escaping="yes"&gt;</xsl:text>
				<xsl:text disable-output-escaping="yes">&amp;lt;/xsl:transform&amp;gt;</xsl:text> 
				<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 

			<xsl:text>&#10;</xsl:text>
			<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:template&gt;</xsl:text>
			<xsl:text>&#10;</xsl:text>
			<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:transform&gt;</xsl:text>
		</xsl:template>
</xsl:transform>
