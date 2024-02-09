
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->



    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">1.4</xsl:attribute>
    </xsl:template>


    <!-- Add ATTRIBUTES CollaborationConfig ( were absent in 'objectXml_1.3' but added to  'objectXml_1.4') -->

    <xsl:template mode="add-atts" match="/CollaborationConfig">
        <xsl:choose>
            <xsl:when test="@Ver">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="Ver">1.3</xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@NID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="NID"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@Rev">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="Rev">1</xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
            <xsl:when test="@parentNID">
            </xsl:when>
            <xsl:otherwise>
                <xsl:attribute name="parentNID"></xsl:attribute>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template mode="append" match="/CollaborationConfig">
        <xsl:choose>
            <xsl:when test="Groups">
            </xsl:when>
            <xsl:otherwise>
                <Groups>
                    <!-- <Tag Name="" Value=""/>  -->
                </Groups>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>