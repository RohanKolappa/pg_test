<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
     <MediaRoomRoleSet>
        <xsl:apply-templates/>
     </MediaRoomRoleSet> 
    </xsl:template>

    <xsl:template match="record">
        <MediaRoomObjectRole>
          <RoleObject><xsl:value-of select="@jsxroleObject"/></RoleObject>
          <RoleAction><xsl:value-of select="@jsxroleAction"/></RoleAction>
        </MediaRoomObjectRole>
    </xsl:template>

 </xsl:stylesheet>
