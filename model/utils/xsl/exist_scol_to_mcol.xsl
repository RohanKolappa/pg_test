<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="no"/>
<xsl:param name="ipvscolprefix"/>
<xsl:template match="/"><xsl:value-of select="$ipvscolprefix"/>.<xsl:value-of select="/*/@parentNID"/>/<xsl:value-of select="substring-after(/*/@NID,concat(/*/@parentNID,'.'))"/></xsl:template>
</xsl:stylesheet>
