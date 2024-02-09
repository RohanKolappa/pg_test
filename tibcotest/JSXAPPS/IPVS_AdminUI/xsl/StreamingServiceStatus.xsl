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
    <table style="table-layout:fixed;width:100%;font-family:verdana,arial,sans-serif;font-size:11px;color:#333333;	border-width: 1px;border-color:#999999;border-collapse: collapse">
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
    <td style="border-width:1px;padding:4px;border-style:solid;border-color:#999999;background-color:#efefef;text-align:right;word-wrap:break-word;"><xsl:value-of select="@name"/></td>
    <td style="border-width:1px;padding:4px;border-style:solid;border-color:#999999;background-color:#ffffff;text-align:left;font-weight:bold;word-wrap:break-word;" width="124"><xsl:value-of select="."/></td>
  </xsl:template>
  
</xsl:stylesheet>