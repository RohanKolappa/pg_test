<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="no"/>

        <xsl:template match="/">
                <StreamStatusEvent>
	               <xsl:apply-templates select="//SourceEventData/*"/>
                          </StreamStatusEvent>
        </xsl:template>
        <xsl:template match="*">
                        <xsl:copy-of select="."/>
      </xsl:template>
</xsl:stylesheet>

