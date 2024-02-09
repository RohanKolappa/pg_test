<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:param name="rootname"/>
	<xsl:output method="xml" version="1.0" encoding="ISO-8859-1" indent="yes" />
		<xsl:template match="/">
			<xsl:text disable-output-escaping="yes">&lt;xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"&gt;</xsl:text>
			<xsl:text>&#10;</xsl:text>
			<xsl:text disable-output-escaping="yes">&lt;xsl:output method="text"&#47;&gt;</xsl:text>
			<xsl:text>&#10;</xsl:text>
				<xsl:text disable-output-escaping="yes">&lt;xsl:template match="/"&gt;</xsl:text>
				<xsl:for-each select="//handler[action='MODIFY' or action='ADD' ][device=$rootname]">
					<xsl:text>&#10;</xsl:text>
					<xsl:text disable-output-escaping="yes">&lt;xsl:for-each select="</xsl:text> 
					<xsl:value-of select="triggername"/>
					<xsl:text disable-output-escaping="yes">"&gt;</xsl:text> 
					<xsl:text>&#10;</xsl:text>

					<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
					<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
					<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
					<xsl:text>&#10;</xsl:text>
					<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
					<xsl:value-of select="cmd"/>
					<xsl:text disable-output-escaping="yes"> &lt;&#47;xsl:text&#62;</xsl:text> 
					<xsl:if test="parent">
						<xsl:text>&#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;-</xsl:text>
						<xsl:value-of select="parent/@cliargument"/>
						<xsl:text disable-output-escaping="yes"> &lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;xsl:value-of select="</xsl:text> 
						<xsl:value-of select="parent/@relativeXpath"/>
						<xsl:text disable-output-escaping="yes">"&#47;&gt;</xsl:text> 
					</xsl:if>

					<xsl:for-each select="paramtable/paramentry">

						<xsl:text>&#10;</xsl:text>
						<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt; -</xsl:text>
						<xsl:value-of select="paramname"/>
						<xsl:text disable-output-escaping="yes"> &lt;&#47;xsl:text&#62;</xsl:text> 
						<xsl:text>&#10;</xsl:text>
						
						<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;"&lt;&#47;xsl:text&gt;</xsl:text>
						<xsl:variable name="Type" select="@type" />
						<xsl:choose>
							<xsl:when test='"URLString"=$Type'>
								<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;`echo "&lt;&#47;xsl:text&gt;</xsl:text>
								<xsl:text>&#10;</xsl:text>
								<xsl:text disable-output-escaping="yes">&lt;xsl:value-of select="</xsl:text>                
								<xsl:value-of select="paramval"/>
								<xsl:text disable-output-escaping="yes">"&#47;&#62;</xsl:text>
								<xsl:text>&#10;</xsl:text>
								<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;" | urlencoder`&lt;&#47;xsl:text&gt;</xsl:text>
							</xsl:when>
							<xsl:otherwise>
								<xsl:text disable-output-escaping="yes">&lt;xsl:value-of select="</xsl:text>
								<xsl:value-of select="paramval"/>
								<xsl:text disable-output-escaping="yes">"&#47;&#62;</xsl:text>
							</xsl:otherwise>
						</xsl:choose>
						<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;"&lt;&#47;xsl:text&gt;</xsl:text>
						
					</xsl:for-each>
					<xsl:text>&#10;</xsl:text>
<!--
					<xsl:text disable-output-escaping="yes">&lt;xsl:text&gt;</xsl:text>
					<xsl:text disable-output-escaping="yes">&amp;#10;</xsl:text>
					<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:text&#62;</xsl:text> 
-->
					<xsl:text>&#10;</xsl:text>
					<xsl:text disable-output-escaping="yes">&#60;&#47;xsl:for-each&#62;</xsl:text> 
				</xsl:for-each>
			<xsl:text>&#10;</xsl:text>
			<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:template&gt;</xsl:text>
			<xsl:text>&#10;</xsl:text>
			<xsl:text disable-output-escaping="yes">&lt;&#47;xsl:transform&gt;</xsl:text>
		</xsl:template>
</xsl:transform>
