<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <xsl:output method="xml" indent="yes"/>

	 <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.25.6</xsl:attribute>
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
    
     <xsl:template match="//DisplayProfile/Canvas/Background" mode="after">
	   <Layout>
	      <xsl:attribute name="id"><xsl:text>layoutId1</xsl:text></xsl:attribute>
	      <xsl:attribute name="type"><xsl:text>Custom</xsl:text></xsl:attribute>
	      <xsl:attribute name="border"><xsl:text>hide</xsl:text></xsl:attribute>
	      <xsl:attribute name="borderStyle"><xsl:text>[8px]</xsl:text></xsl:attribute>
	      <xsl:attribute name="header"><xsl:text>hide</xsl:text></xsl:attribute>
	      <xsl:attribute name="headerStyle"><xsl:text>basic</xsl:text></xsl:attribute>
	      <xsl:attribute name="active"><xsl:text>true</xsl:text></xsl:attribute>
	   </Layout> 
 	</xsl:template>
 	<xsl:template match="//DisplayProfile/Canvas/Window/@enable"/>	
	<xsl:template match="//DisplayProfile/Canvas/Window/ConnectionContent" mode='add-atts'>
	  <xsl:attribute name="vres"></xsl:attribute>	
	  <xsl:attribute name="hres"></xsl:attribute>	
	  <xsl:attribute name="srcKbmCtrl"></xsl:attribute>	    
	</xsl:template>	
	<xsl:template match="//DisplayProfile/Canvas/Window/Geometry">
	 <Properties>
	    <xsl:apply-templates select="@* | node()"/>	
	      <xsl:attribute name="zIndex"><xsl:text>100</xsl:text></xsl:attribute>	
	      <xsl:attribute name="layoutId"><xsl:text>layoutId1</xsl:text></xsl:attribute>
	      <xsl:attribute name="layoutPos"><xsl:text></xsl:text></xsl:attribute>
	      <xsl:attribute name="visibility"><xsl:text>true</xsl:text></xsl:attribute>
	      <xsl:attribute name="maximize"><xsl:text>false</xsl:text></xsl:attribute>
	      <xsl:attribute name="fps"><xsl:text>15</xsl:text></xsl:attribute>
	      <xsl:attribute name="scale"><xsl:text>fitWindow</xsl:text></xsl:attribute>
	      <xsl:attribute name="mute"><xsl:text>false</xsl:text></xsl:attribute>
	      <xsl:attribute name="volume"><xsl:text>50%</xsl:text></xsl:attribute>
	      <xsl:attribute name="remoteKbmCtrl"><xsl:text>false</xsl:text></xsl:attribute>
	      <xsl:attribute name="scrollHPos"><xsl:text>0</xsl:text></xsl:attribute>
	      <xsl:attribute name="scrollVPos"><xsl:text>0</xsl:text></xsl:attribute>
	 </Properties>
	</xsl:template>	

</xsl:stylesheet>
 