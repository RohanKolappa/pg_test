<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" indent="no"/>

  <xsl:template match="UserPolicyList">
    <xsl:for-each select="./User">
      <xsl:text>User, </xsl:text>
     
      <xsl:value-of select="@username" />
      <xsl:text>, </xsl:text>
     
      <xsl:value-of select="@fullName" />
      <xsl:text>, </xsl:text>
    
      <xsl:value-of select="@creationDate" />
      <xsl:text>, </xsl:text>
    
      <xsl:for-each select="./Groups/Tag[@Name='Location']">
        <xsl:value-of select="@Value" />
        <xsl:text>:</xsl:text>
      </xsl:for-each>
      
      <xsl:text>, </xsl:text>
     
      <xsl:for-each select="./Permissions/User/Action[@name='Add']/Groups/Tag">
        <xsl:value-of select="@Name" />
        <xsl:text>=</xsl:text>
        
        <xsl:value-of select="@Value" />
        <xsl:text>:</xsl:text>
      </xsl:for-each>
        
     <xsl:text>&#xa;</xsl:text>
   </xsl:for-each>
        
  </xsl:template>
  
</xsl:stylesheet>
