<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<!-- dbtemplate : include xsl template -->
	<xsl:import href="../../../shared/xsl/_dbtemplate.xsl" />
	<!-- add template here for object specific -->

	<xsl:template match="@serviceVer">
		<xsl:attribute name="serviceVer">2.32.5</xsl:attribute>
	</xsl:template>
	<xsl:template match="@Ver">
		<xsl:attribute name="Ver">2.5</xsl:attribute>
	</xsl:template>

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

	<xsl:template
		match="/MediaStreamRelayPortResource/Info/StreamTypeList/StreamType">
		<xsl:variable name="streamTypeValue" select="." />
		<xsl:choose>
			<xsl:when test="Inout">
				<xsl:copy-of select="." />
			</xsl:when>
			<xsl:otherwise>
				<StreamType>
					<Inout>
						<xsl:choose>
							<xsl:when test="$streamTypeValue='V2D'">
								<In>
									<Property name="Protocol" value="V2D" />
									<Property name="Container" value="RTP" />
									<Property name="VideoEncoding" value="V2D" />
									<Property name="AudioEncoding" value="RAW" />
									<Property name="KBM" value="V2D" />
									<Property name="LegacyStreamType" value="V2D" />
								</In>
								<Out>
									<Property name="Protocol" value="V2D" />
									<Property name="Container" value="RTP" />
									<Property name="VideoEncoding" value="V2D" />
									<Property name="AudioEncoding" value="RAW" />
									<Property name="KBM" value="V2D" />
									<Property name="LegacyStreamType" value="V2D" />
								</Out>
							</xsl:when>
							<xsl:when test="$streamTypeValue='MPEGTS'">
								<In>
									<Property name="Protocol" value="UDP" />
									<Property name="Container" value="TS" />
									<Property name="VideoEncoding" value="MPEG2" />
									<Property name="AudioEncoding" value="MPEG2/3" />
									<Property name="LegacyStreamType" value="MPEGTS" />
								</In>
								<Out>
									<Property name="Protocol" value="UDP" />
									<Property name="Container" value="TS" />
									<Property name="VideoEncoding" value="MPEG2" />
									<Property name="AudioEncoding" value="MPEG2/3" />
									<Property name="LegacyStreamType" value="MPEGTS" />
								</Out>
							</xsl:when>
							<xsl:when test="$streamTypeValue='RTP'">
								<In>
									<Property name="Protocol" value="SDPText" />
									<Property name="Container" value="RTP" />
									<Property name="VideoEncoding" value="Any" />
									<Property name="AudioEncoding" value="Any" />
									<Property name="LegacyStreamType" value="RTP" />
								</In>
								<Out>
									<Property name="Protocol" value="SDPURL" />
									<Property name="Container" value="RTP" />
									<Property name="VideoEncoding" value="Any" />
									<Property name="AudioEncoding" value="Any" />
									<Property name="LegacyStreamType" value="RTP" />
								</Out>
							</xsl:when>
							<xsl:when test="$streamTypeValue='UDP'">
								<In>
									<Property name="Protocol" value="UDP" />
									<Property name="Container" value="RAW" />
									<Property name="LegacyStreamType" value="UDP" />
								</In>
								<Out>
									<Property name="Protocol" value="UDP" />
									<Property name="Container" value="RAW" />
									<Property name="LegacyStreamType" value="UDP" />
								</Out>
							</xsl:when>
						</xsl:choose>
					</Inout>
				</StreamType>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>