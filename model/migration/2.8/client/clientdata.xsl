<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" indent="yes"/>

  <xsl:template match="/*/@serviceVer">
    <xsl:attribute name="serviceVer">
      <xsl:text>2.9</xsl:text>
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

  <!--Add Elements 'Priority' -->
  <xsl:template mode="append" match="/Preferences/Player">
    <xsl:choose>
      <xsl:when test="Priority">
        <!-- do noting -->  
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
        <Priority>false</Priority>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template  match="/Preferences/ClientApp[@name='IPVSClient']/ShowBookmarks">
	<!-- set value to false --> 
        <ShowBookmarks>false</ShowBookmarks>
  </xsl:template>
  <xsl:template  match="/Preferences/ClientApp[@name='IPVSClient']/ShowConnections">
	<!-- set value to false --> 
        <ShowConnections>false</ShowConnections>
  </xsl:template>



  <!--Add Change 'ShowBookmarks' & 'ShowConnections' Default value to 'false'-->
  <xsl:template mode="append" match="/Preferences/ClientApp[@name='IPVSClient']">
    <!-- bookmarks -->
    <xsl:choose>
      <xsl:when test="ShowBookmarks">
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
	<ShowBookmarks>false</ShowBookmarks>
      </xsl:otherwise>
    </xsl:choose>

    <!-- Connections -->
   <xsl:choose>
      <xsl:when test="ShowConnections">
      </xsl:when>
      <xsl:otherwise>
        <!-- add node -->
	<ShowConnections>false</ShowConnections>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  

  <!--Set element 'AssetPrivateKeyScheme' default value to 'SERVER' -->
  <xsl:template  match="/Preferences/Asset/AssetPrivateKeyScheme">
	<!-- set value to 'SERVER' --> 
        <AssetPrivateKeyScheme>SERVER</AssetPrivateKeyScheme>
  </xsl:template>
  
  <!--Defensive add element 'AssetPrivateKeyScheme' under 'Asset' -->
  <xsl:template mode="append" match="/Preferences/Asset">
    <!-- AssetPrivateKeyScheme -->
    <xsl:choose>
      <xsl:when test="AssetPrivateKeyScheme">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'AssetPrivateKeyScheme' node -->
	<AssetPrivateKeyScheme>SERVER</AssetPrivateKeyScheme>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  
  <!--Add element 'AssetPrivateKeyData' under 'Asset' -->
  <xsl:template mode="append" match="/Preferences/Asset">
    <!-- AssetPrivateKeyData -->
    <xsl:choose>
      <xsl:when test="AssetPrivateKeyData">
      </xsl:when>
      <xsl:otherwise>
        <!-- add 'AssetPrivateKeyData' node -->
	<AssetPrivateKeyData></AssetPrivateKeyData>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

