<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>
   
   <xsl:template match="//Status">
    <html>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="Parameters">
    <table border="1" style="font-family:Arial;font-size:9px;">
      <xsl:for-each select="Parameter[(position()-1) mod 3 = 0]">
        <tr>
          <xsl:apply-templates select="."/>
          <xsl:apply-templates select="following-sibling::*[1]"/>
          <xsl:apply-templates select="following-sibling::*[2]"/>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>
  
  <xsl:template match="Parameter">
    <td align="right"><xsl:value-of select="@name"/></td>
    <td style="font-weight:bold;" align="left"><xsl:value-of select="."/></td>
  </xsl:template>
  
</xsl:stylesheet>