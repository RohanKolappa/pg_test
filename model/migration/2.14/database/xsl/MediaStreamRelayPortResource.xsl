
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <!-- dbtemplate :  include xsl template-->
   <xsl:import href="../../../shared/xsl/_dbtemplate.xsl"/>
   <!-- add template here for object specific -->

    <xsl:template match="@serviceVer">
        <xsl:attribute name="serviceVer">2.15</xsl:attribute>
    </xsl:template>  

    <xsl:template mode="append" match="//InstanceInfo/InstanceList">
       <xsl:choose>
            <xsl:when test="ResourceInstance">
            </xsl:when>
            <xsl:otherwise>
				<ResourceInstance>
					<ResourceNID />			
					<UpstreamMediaStreamInfo>
						<MediaStreamURL />
						<MediaStreamList />
						<MediaStreamProfileInfo />
					</UpstreamMediaStreamInfo>	
					<DownstreamMediaStreamInfo>
						<MediaStreamURL />
						<MediaStreamList />
						<MediaStreamProfileInfo />
					</DownstreamMediaStreamInfo>				
		           <DeviceStreamingServiceReceiveInstanceNID/>
                   <DeviceStreamingServiceTransmitInstanceNID/>
				</ResourceInstance>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>
