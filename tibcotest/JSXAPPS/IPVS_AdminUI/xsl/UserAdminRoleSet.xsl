<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <UserAdminRoleSet>
        <xsl:apply-templates/>
     </UserAdminRoleSet> 
    </xsl:template>

    <xsl:template match="record">
        <UserAdminObjectRole>
          <RoleObject><xsl:value-of select="@jsxroleObject"/></RoleObject>
          <RoleAction><xsl:value-of select="@jsxroleAction"/></RoleAction>
        </UserAdminObjectRole>
    </xsl:template>

 </xsl:stylesheet>
