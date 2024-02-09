<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    
    <xsl:param name="aftermigration_serviceVersion"/>
    <!--NOTE: NOTHING MUCH CHANGED -->

    <!-- delete nid-scheme attributes -->
    <xsl:template match="@Parse"/>
    <xsl:template match="@NIDScheme"/>
    <xsl:template match="@noNamespaceSchemaLocation"/>
    <xsl:template match="@KeyElementRelativeXPath"/>
    <xsl:template match="/*//*/@NID"/>
    <xsl:template match="/*//Tag[@Name='']"/>
    <xsl:template match="/*//MatchOp[.='EqualTo']">
    	<MatchOp>=</MatchOp>
    </xsl:template>
    <xsl:template match="/*//MatchOp[.='NotEqualTo']">
    	<MatchOp>!=</MatchOp>
    </xsl:template> 
    
    
	<!-- Ver and Rev must be defined by calling   -->
    <!-- if exists serviceVer -->
    <xsl:template match="/*/@serviceVer">
        <xsl:attribute name="serviceVer"><xsl:value-of select="$aftermigration_serviceVersion"></xsl:value-of> </xsl:attribute>
    </xsl:template>
    <!-- if exists Version -->
    <xsl:template match="/*/@Version">
        <xsl:attribute name="Ver"><xsl:value-of select="$Ver"></xsl:value-of></xsl:attribute>
    </xsl:template>
    <!-- if exists Ver-->
    <xsl:template match="/*/@Ver">
        <xsl:attribute name="Ver"><xsl:value-of select="$Ver"></xsl:value-of></xsl:attribute>
    </xsl:template>
    <!-- if exists Ver-->
    <xsl:template match="/*/@Rev">
        <xsl:attribute name="Rev"><xsl:value-of select="$Rev"></xsl:value-of></xsl:attribute>
    </xsl:template>


    <!--Add ATTRIBUTES AssetAdminPolicyRule ( were absent in 'objectXml_1.0' but added to 'objectXml_1.3')-->
    <xsl:template name="addAttrs_rootEL">
      <xsl:choose>
            <xsl:when test="@Ver">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="Ver"><xsl:value-of select="$Ver"></xsl:value-of></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@Rev">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="Rev"><xsl:value-of select="$Rev"></xsl:value-of></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@parentNID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="parentNID"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@serviceVer">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="serviceVer"><xsl:value-of select="$aftermigration_serviceVersion"></xsl:value-of></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template> 

</xsl:stylesheet>