<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" indent="yes"/>
   
   <xsl:template match="/">
    <html>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="Parameters">
        <table style="table-layout:fixed;font-size:12px;margin:0px;width:100%;text-align:left;border-collapse:collapse;color:#333;border: solid 1px #777777;">
            <thead>
                <tr>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;" width="350">Name</th>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;" width="150">Health Status</th>
                    <th style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;width:100%;">Disk Usage</th>
                </tr>
            </thead>
                <tbody>
                 <tr valign="top">
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;" class="t1">
                       <xsl:value-of select="./Parameter[@name='Name']"/>
                    </td>
                    <td style="padding: 4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">
                        <xsl:variable name="raidStatus"><xsl:value-of select="./Parameter[@name='Raid Status']"/></xsl:variable>
                        <xsl:choose>
                          <xsl:when test="$raidStatus='OK'">
                             <div style="text-align:center;color:#02E93A"><xsl:value-of select="./Parameter[@name='Raid Status']"/></div>
                          </xsl:when>
                          <xsl:when test="$raidStatus='ERROR'">
                             <div style="text-align:center;color:#E90202"><xsl:value-of select="./Parameter[@name='Raid Status']"/></div>
                          </xsl:when>
                          <xsl:otherwise>
                             <div style="text-align:center;color:#FF8000"><xsl:value-of select="./Parameter[@name='Raid Status']"/></div>
                          </xsl:otherwise>
                        </xsl:choose>
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
                <tr>   
                    <td colspan="4" style="text-align:center;padding:10px;background:#f5f5f5;border-top:solid 1px #999999;border-right:solid 1px #999999;">
                        <table style="color:#555555;margin:0px;width:100%;text-align:center;border-collapse:collapse;border:solid 1px #777777;">
                            <tr> 
                                <td colspan="9" style="font-size: 13px;font-weight: normal;padding: 2px 4px 2px 4px;background: #e4e4e4;background-image: url(H2BG.png); background-position:center; background-repeat:repeat-x;border-bottom: solid 1px #777777;text-align:center;color: #121212;">RAID Info:-
                                                        RAID Name:<xsl:value-of select="./Parameter[@name='RAID']" />
                                                        BBU Status:<xsl:value-of select="./Parameter[@name='BBU Status']" />
                                </td>
                            </tr>
                            <tr>
                                <td style="color:#121212;text-align:center;background-color:#e5e5e5;padding:0px 4px 0px 4px;">Disk</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 0</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 1</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 2</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 3</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 4</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 5</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 6</td>
                                <td style="color:#121212;padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;">Disk 7</td>

                            </tr>
                            <tr>
                               <td style="color:#121212;text-align:center;background-color:#e5e5e5;padding:0px 4px 0px 4px;">Serial#</td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #0']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #1']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #2']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #3']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #4']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #5']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #6']" /></td>
                               <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Disk #7']" /></td>
                            </tr>
                            <tr>
                                <td style="color:#121212;text-align:center;background-color:#e5e5e5;padding:0px 4px 0px 4px;">Status</td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #0'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #1'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #2'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #3'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #4'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #5'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #6'and@type='String']"/></td>
                                <td style="padding:4px 4px 4px 4px;background:#f5f5f5;text-align:center;border-top:solid 1px #999999;border-right:solid 1px #999999;"><xsl:value-of select="./Parameter[@name='Status #7'and@type='String']"/></td>
                            </tr>
                      </table>  
                    </td>
                </tr>
            </tbody>
        </table>
        <br />
  </xsl:template>
</xsl:stylesheet>
