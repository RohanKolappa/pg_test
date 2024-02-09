<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:rz1="http://exist.sourceforge.net/NS/exist" >
<xsl:output method="text" omit-xml-declaration="yes" standalone="yes" indent="no"/>
<xsl:param name="collection_name"/>
<xsl:param name="folderPath"/>
<xsl:variable name="amp" >&amp;
</xsl:variable>
<xsl:template match="/">
<xsl:for-each select="//rz1:resource[@name!='collection.xconf']">
<xsl:choose>
<xsl:when test="string-length($collection_name)=0">declare boundary-space preserve; LOAD '<xsl:value-of select="$folderPath" />/<xsl:value-of select="@name" />'  '<xsl:value-of select="@name" />'<xsl:value-of select="$amp"/></xsl:when>
<xsl:otherwise>declare boundary-space preserve; LOAD '<xsl:value-of select="$folderPath" />/<xsl:value-of select="@name"/>'  '<xsl:value-of select="@name" />'  '<xsl:value-of select="$collection_name"/>'<xsl:value-of select="$amp"/></xsl:otherwise></xsl:choose>
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
 