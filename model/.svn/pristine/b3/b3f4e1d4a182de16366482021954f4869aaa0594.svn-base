<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="/*/@serviceVer">
		<xsl:attribute name="serviceVer">
		<xsl:text>2.34.2</xsl:text>
	</xsl:attribute>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'MS') or starts-with(name(),'DMS')]/*/SystemElementsConfig/KeyboardConfig">
		<xsl:copy>
			<xsl:apply-templates select="@*" />
			<xsl:element name="Layout">
				<xsl:text>us</xsl:text>
			</xsl:element>
			<xsl:element name="Variant">
				<xsl:text>intl</xsl:text>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="*[starts-with(name(),'MS') or starts-with(name(),'DMS')]/*/SystemElementsConfig/RegionalSettingsConfig">
		<xsl:copy>
			<xsl:apply-templates select="@*" />
			<xsl:element name="Locale">
				<xsl:text>en_US</xsl:text>
			</xsl:element>
			<xsl:element name="Timezone">
				<xsl:text>America/Los_Angeles</xsl:text>
			</xsl:element>

		</xsl:copy>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'MS') or starts-with(name(),'DMS')]/*/SystemElementsConfig/HTTPModeConfig">
		<xsl:copy>
			<xsl:apply-templates select="@*" />
			<xsl:element name="Enable">
				<xsl:text>true</xsl:text>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="//HAServiceConfig">
		<xsl:copy>
			<xsl:attribute name="Dirty">CLEAN</xsl:attribute>
			<xsl:apply-templates select="node()|@*" />
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'EMS') or starts-with(name(),'NGS-D220')]/*/NetworkElementsStatus/EthernetPortTableStatus/EthernetPortStatus/Parameters">
		<ParametersList>
			<xsl:copy>
				<xsl:apply-templates select="@*|node()" />
			</xsl:copy>
		</ParametersList>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'OWS')]/*/ServiceElementsConfig/OWSServiceConfig/Config/hid">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()" />
			<xsl:element name="keyboard_connected">
				<xsl:text>false</xsl:text>
			</xsl:element>
			<xsl:element name="mouse_connected">
				<xsl:text>false</xsl:text>
			</xsl:element>
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'OWS')]/*/ServiceElementsConfig/OWSServiceConfig/Config/audio">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()" />
			<xsl:element name="speaker_connected">
				<xsl:text>false</xsl:text>
			</xsl:element>			
		</xsl:copy>
	</xsl:template>
	<xsl:template match="*[starts-with(name(),'OWS')]/*/ServiceElementsConfig/OWSServiceConfig/Config/displays/displays/topology">
		<xsl:copy>
			<xsl:apply-templates select="@*|node()" />
			<xsl:element name="rotation">
				<xsl:text>0</xsl:text>
			</xsl:element>			
		</xsl:copy>
	</xsl:template>
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


	<!-- copy device status only if serviceVer is of last migration -->
	<xsl:template match="//DeviceStatus[not(*)]">
		<xsl:variable name="rootname" select="name(/*)" />
		<xsl:variable name="objectxml_file">
			<xsl:value-of select="concat('/home/onair/objectXml/',$rootname,'.xml')" />
		</xsl:variable>
		<xsl:variable name="objectXml_serviceVer"
			select="document($objectxml_file)/*/@serviceVer" />
		<xsl:choose>
			<xsl:when test='"2.34.2"=$objectXml_serviceVer'>
				<xsl:copy-of select="document($objectxml_file)//DeviceStatus" />
			</xsl:when>
			<xsl:otherwise>
				<DeviceStatus>
					skip copy
					target serviceVer
					<xsl:value-of select="$objectXml_serviceVer" />
					in objectXml
					<xsl:value-of select="$objectxml_file" />
				</DeviceStatus>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


</xsl:stylesheet>

