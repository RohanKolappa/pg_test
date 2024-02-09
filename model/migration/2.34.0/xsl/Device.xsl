<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" indent="yes" />

	<xsl:template match="/*/@serviceVer">
		<xsl:attribute name="serviceVer">
		<xsl:text>2.34.1</xsl:text>
	</xsl:attribute>
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
			<xsl:when test='"2.34.1"=$objectXml_serviceVer'>
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
	<xsl:template match="//V2D-XPi-Device/*/SystemElementsConfig/LoginBannerConfig/Banner">
	<Banner>

                                    PPPPPPPPPPPPPPPPP   
                                    P::::::::::::::::P  
                                    P::::::PPPPPP:::::P 
                                    PP:::::P     P:::::P
                xxxxxxx      xxxxxxx  P::::P     P:::::P
                 x:::::x    x:::::x   P::::P     P:::::P
                  x:::::x  x:::::x    P::::PPPPPP:::::P 
                   x:::::xx:::::x     P:::::::::::::PP  
                    x::::::::::x      P::::PPPPPPPPP    
                     x::::::::x       P::::P            
                     x::::::::x       P::::P            
                    x::::::::::x      P::::P            
                   x:::::xx:::::x   PP::::::PP          
                  x:::::x  x:::::x  P::::::::P          
                 x:::::x    x:::::x P::::::::P          
                xxxxxxx      xxxxxxxPPPPPPPPPP          

			</Banner>
	</xsl:template>
	<xsl:template mode="append"
		match="//V2D-XPi-Device/*/SystemElementsConfig">
		<CustomSplashConfig Dirty="CLEAN">
			<Enable>false</Enable>
			<LoadCustomImage>false</LoadCustomImage>
			<ImageURL></ImageURL>
		</CustomSplashConfig>
	</xsl:template>
	<xsl:template mode="append"
		match="//V2D-XPi-Device/*/AVCodecElementsConfig/XP200PortConfig">
		<XP200EncoderRegionOfInterestConfig
			Dirty="CLEAN">
			<Enable>false</Enable>
			<Width>0</Width>
			<Height>0</Height>
			<Height>0</Height>
			<HorizontalOffset>0</HorizontalOffset>
			<VerticalOffset>0</VerticalOffset>
		</XP200EncoderRegionOfInterestConfig>
	</xsl:template>

</xsl:stylesheet>

