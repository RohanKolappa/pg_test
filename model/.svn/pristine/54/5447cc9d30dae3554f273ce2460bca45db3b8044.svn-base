<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>

<xsl:template match="/*/@serviceVer">
  <xsl:attribute name="serviceVer">
    <xsl:text>2.28.11</xsl:text>
  </xsl:attribute>
</xsl:template>

<!-- Here's the boilerplate code -->
<!-- By default, copy all nodes unchanged -->
<xsl:template match="@* | node()">
  <xsl:apply-templates mode="before" select="."/>
  <xsl:copy>
    <xsl:apply-templates select="@*"/>
    <xsl:apply-templates mode="add-atts" select="."/>
    <xsl:apply-templates mode="insert" select="."/>
    <xsl:apply-templates/>
    <xsl:apply-templates mode="append" select="."/>
  </xsl:copy>
  <xsl:apply-templates mode="after" select="."/>
</xsl:template>

<!-- By default, don't add anything -->
<xsl:template mode="add-atts" match="*"/>
<xsl:template mode="insert" match="*"/>
<xsl:template mode="append" match="*"/>
<xsl:template mode="before" match="@* | node()"/>
<xsl:template mode="after" match="@* | node()"/>

<xsl:strip-space elements="*" /> 

<!--add element 'ServerList'  and 'LastLoginServer' under 'Login' -->
 <xsl:template mode="append" match="/Preferences/Login">  
    <!-- LastLoginServer -->
    <xsl:choose>
      <xsl:when test="LastLoginServer">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'LastLoginServer' node -->
	    <LastLoginServer></LastLoginServer>
      </xsl:otherwise>
    </xsl:choose> 
    <!-- ServerList -->
    <xsl:choose>
      <xsl:when test="ServerList">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'ServerList' node -->
	    <ServerList>
		<Server>
		  <xsl:attribute name="Name">    		
  		  </xsl:attribute>
		  <xsl:attribute name="Value">
		     <xsl:choose>
		     <!-- Copy the server to ServerList as first entry -->
                     <xsl:when test="/Preferences/Login/server">
			  <xsl:value-of select="/Preferences/Login/server" />
      			</xsl:when>			
		     </xsl:choose>    		    
  		  </xsl:attribute>
		</Server>
	    </ServerList>
      </xsl:otherwise>
     </xsl:choose>    
 </xsl:template>

 <xsl:template match="/Preferences/Login/server"/> 


</xsl:stylesheet>

