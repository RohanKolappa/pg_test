<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="User">
        <data jsxid="jsxroot">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
        </data>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@NID"/>
        </xsl:attribute>

        <xsl:attribute name="Username">
            <xsl:value-of select="$record_el/@username"/>
        </xsl:attribute>
        <xsl:attribute name="ExternalAuthentication">
            <xsl:value-of select="$record_el/@externalAuthentication"/>
        </xsl:attribute>
        <xsl:attribute name="Password">
            <xsl:value-of select="$record_el/@password"/>
        </xsl:attribute> 
        <xsl:attribute name="VerifyPassword">
            <xsl:value-of select="$record_el/@password"/>
        </xsl:attribute> 
        <xsl:attribute name="Email">
            <xsl:value-of select="$record_el/@email"/>
        </xsl:attribute> 
        <xsl:attribute name="Name">
            <xsl:value-of select="$record_el/@fullName"/>
        </xsl:attribute> 
        <xsl:attribute name="UserPrivateData">
            <xsl:value-of select="$record_el/UserPrivateData"/>
        </xsl:attribute> 
        <xsl:attribute name="CreationDate">
            <xsl:value-of select="$record_el/@creationDate"/>
        </xsl:attribute> 
        <xsl:attribute name="ModifiedDate">
            <xsl:value-of select="$record_el/@modifiedDate"/>
        </xsl:attribute> 
     </xsl:template>
</xsl:stylesheet>