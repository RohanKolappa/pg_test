<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" omit-xml-declaration="yes" encoding="UTF-8"/>
    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="record">
          <xsl:attribute name="NID">
              <xsl:value-of select="@NID"/>
          </xsl:attribute>
          <DeviceConfig>
            <xsl:attribute name="Dirty">
                <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
            </xsl:attribute>
              <ServiceElementsConfig>
                <StreamingServiceConfig>
                  <StreamsTxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MulticastIpAddressRange>
                      <From>
                          <xsl:value-of select="@MulticastAddrRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@MulticastAddrRangeTo"/>
                      </To>
                      <MulticastTTL>
                          <xsl:value-of select="@MulticastTTL"/>
                      </MulticastTTL>
                    </MulticastIpAddressRange>
                  </StreamsTxConfig>
                  <V2DStreamsTxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MaxStreamInstanceCount/>
                    <TxCtrlPortRange>
                      <From>
                          <xsl:value-of select="@CtrlPortRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@CtrlPortRangeTo"/>
                      </To>
                    </TxCtrlPortRange>
                  </V2DStreamsTxConfig>
                  <V2DStreamsRxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MaxStreamInstanceCount/>
                    <RxMediaPortRange>
                      <From>
                          <xsl:value-of select="@AVPortRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@AVPortRangeTo"/>
                      </To>
                    </RxMediaPortRange>
                  </V2DStreamsRxConfig>
                  <MPEGTSStreamsRxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MaxStreamInstanceCount/>
                    <RxMediaPortRange>
                      <From>
                          <xsl:value-of select="@MPEGTSAVPortRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@MPEGTSAVPortRangeTo"/>
                      </To>
                    </RxMediaPortRange>
                  </MPEGTSStreamsRxConfig>
                  <RTPStreamsRxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MaxStreamInstanceCount/>
                    <RxMediaPortRange>
                      <From>
                          <xsl:value-of select="@RTPAVPortRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@RTPAVPortRangeTo"/>
                      </To>
                    </RxMediaPortRange>
                  </RTPStreamsRxConfig>
                  <UDPStreamsRxConfig>
                    <xsl:attribute name="Dirty">
                      <xsl:value-of select="@DirtyBitblkAssetStreamSettings"/>
                    </xsl:attribute>
                    <MaxStreamInstanceCount/>
                    <RxMediaPortRange>
                      <From>
                          <xsl:value-of select="@UDPAVPortRangeFrom"/>
                      </From> 
                      <To>
                          <xsl:value-of select="@UDPAVPortRangeTo"/>
                      </To>
                    </RxMediaPortRange>
                  </UDPStreamsRxConfig>
                </StreamingServiceConfig>
              </ServiceElementsConfig>
           </DeviceConfig>
    </xsl:template>

 </xsl:stylesheet>
