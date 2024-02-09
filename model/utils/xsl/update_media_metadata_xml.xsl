<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml" indent="yes"/>
    <!-- Here's the boilerplate code -->
    <!-- By default, copy all nodes unchanged -->
    <xsl:template match="@* | node()">
    <xsl:apply-templates mode="before" select="." />
    <xsl:copy>
        <xsl:apply-templates select="@*" />
        <xsl:apply-templates mode="add-atts" select="." />
        <xsl:apply-templates mode="insert" select="." />
        <xsl:apply-templates />
        <xsl:apply-templates mode="append" select="." />
    </xsl:copy>
    <xsl:apply-templates mode="after" select="." />
    </xsl:template>
    <!-- By default, don't add anything -->
    <xsl:template mode="add-atts" match="*" />
    <xsl:template mode="insert" match="*" />
    <xsl:template mode="append" match="*" />
    <xsl:template mode="before" match="@* | node()" />
    <xsl:template mode="after" match="@* | node()" />


    <xsl:param name="filepath"></xsl:param>
    
    <xsl:variable name="BookmarkFile">
		  <xsl:value-of select="$filepath"/>
    </xsl:variable>
    
	  <xsl:template match="/">
		  <xsl:apply-templates/>
	  </xsl:template>
	  
    <xsl:template mode="append" match="//BookmarkList">
      <Bookmark>
        <xsl:attribute name="title">
          <xsl:value-of select="document($BookmarkFile)//Bookmark/Title"/>
        </xsl:attribute>
        
        <xsl:copy-of select="document($BookmarkFile)//Bookmark/DateCreated"/>
        <xsl:copy-of select="document($BookmarkFile)//Bookmark/Comment"/>
        <xsl:copy-of select="document($BookmarkFile)//Bookmark/DateModified"/>
        <xsl:copy-of select="document($BookmarkFile)//Bookmark/Offset"/>
        <TimeStamp><xsl:value-of select="document($BookmarkFile)//Bookmark/AtTC"/></TimeStamp>
        <xsl:copy-of select="document($BookmarkFile)//Bookmark/AuthorUserID"/>
      </Bookmark>

    </xsl:template>
  </xsl:stylesheet>