<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>
   
   <xsl:template match="/">
    <html>
      <body>
        <table style="table-layout:fixed;font-size:12px;margin:0px;width:100%;text-align:left;border-collapse:collapse;color:#333;border: solid 1px #777777;">
            <thead>
                <tr>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;" width="200">NAS Name</th>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;" width="230">Server</th>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;" width="100">Status</th>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;width:100%;">Disk Usage</th>
                </tr>
            </thead>
            <xsl:apply-templates/>
        </table>
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="Parameters">
                <tbody>
                 <tr valign="top">
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;" class="t1">
                       <xsl:value-of select="./Parameter[@name='Name']"/>
                    </td>
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">
                        <xsl:value-of select="./Parameter[@name='Server']"/>
                    </td>
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">
                        <xsl:value-of select="./Parameter[@name='Status']"/>
                    </td>
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;" align="center">
                        <table style="background:#f5f5f5;border:none;" width="350" height="30" cellpadding="0" cellspacing="0">
                            <tr>
                                <td style="font-size:12px;border:none;">
                                    Capacity:<xsl:value-of select="./Parameter[@name='Total']"/>GB
                                </td>
                                <xsl:variable name="percentage"><xsl:value-of select="./Parameter[@name='PercentUsed']" /></xsl:variable>
                                <td style="font-size:12px;border:none;">
                                    <div style="text-align:left;width:200px;height:16px;border:solid 1px #777777">
                                          <div style="width:{$percentage}px;height:16px;background-color:#6699FF;background-image:url(matrixSelect.png);background-position:center;background-repeat:repeat-x;"></div>
                                    </div>
                                </td>
                                <tr>
                                    <td style="font-size:12px;border:none;"></td>
                                    <td style="font-size:10px;color:#555;text-align:center;vertical-align:text-top;"><xsl:value-of select="./Parameter[@name='Used']"/> GB Used  <xsl:value-of select="./Parameter[@name='Available']"/>GB Available</td>
                                </tr>
                            </tr>
                        </table>
                    </td>
                 </tr>
            </tbody>
  </xsl:template>
</xsl:stylesheet>