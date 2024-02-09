<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="EthernetPortTableConfig">
        <data jsxid="jsxroot">
            <xsl:apply-templates/>
        </data>
    </xsl:template>

    <xsl:template match="EthernetPortTableConfig/EthernetPortConfig">
        <record>
            <xsl:call-template name="record_attributes">
                <xsl:with-param name="record_el" select="."/>
            </xsl:call-template>
        </record>
    </xsl:template>

    <xsl:template name="record_attributes">
        <xsl:param name="record_el"/>
        <xsl:attribute name="jsxid">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxDirtyBit">
            <xsl:value-of select="$record_el/@Dirty"/>
        </xsl:attribute>
        <xsl:attribute name="jsxListIndex">
            <xsl:value-of select="$record_el/@ListIndex"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLanPortId">
            <xsl:value-of select="$record_el/PortID"/>
        </xsl:attribute>
        <xsl:choose>
          <xsl:when test="$record_el/EnableInterface='true'">
                <xsl:attribute name="jsxLanIsEnable">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxLanIsEnable">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/IsPrimary='true'">
                <xsl:attribute name="jsxLanIsPrimary">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxLanIsPrimary">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/EnableMulticast='true'">
                <xsl:attribute name="jsxLanMulticast">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxLanMulticast">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$record_el/IPConfig/UseDHCP='true'">
                <xsl:attribute name="jsxLanDhcp">1</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="jsxLanDhcp">0</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:attribute name="jsxLanIPAddress">
            <xsl:value-of select="$record_el/IPConfig/IPAddress"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLanNetmask">
            <xsl:value-of select="$record_el/IPConfig/Netmask"/>
        </xsl:attribute>
        <xsl:attribute name="jsxLanMtu">
            <xsl:value-of select="$record_el/MTU"/>
        </xsl:attribute>        
     </xsl:template>
</xsl:stylesheet>