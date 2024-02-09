<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes"/>   
    
    <xsl:template match="*">
     <xsl:apply-templates/>
    </xsl:template>
        
    
   <xsl:template match="//AgentMessage">   
          <AgentMessage>
              <xsl:attribute name="from"></xsl:attribute>
              <xsl:attribute name="to"><xsl:value-of select="//agentJID"/></xsl:attribute>
              <xsl:attribute name="type"><xsl:value-of select="//agentmessagetype"/></xsl:attribute>
              <xsl:attribute name="logLevel"><xsl:value-of select="//header/loglevel"/></xsl:attribute>
              <xsl:if test="//Request">
              <xsl:call-template name="request" />
              </xsl:if>
              <xsl:if test="//Response">
              <xsl:call-template name="response" />
              </xsl:if>
              
          </AgentMessage>
   </xsl:template>
    
    
    <xsl:template match="//Request" name="request">
          <Request>
          <xsl:attribute name="serviceVer"></xsl:attribute>
          <Header>
         <xsl:attribute name="serviceName"><xsl:value-of select="//header/servicename"/></xsl:attribute>
         <xsl:attribute name="requestName"><xsl:value-of select="//header/requestname"/></xsl:attribute>
         <xsl:attribute name="logLevel"><xsl:value-of select="//header/loglevel"/></xsl:attribute>
         <ClientData><xsl:value-of select="//header/clientdata"></xsl:value-of></ClientData>
         <ClientCallback></ClientCallback>
        </Header> 
        <Data><xsl:copy-of select="//data/*"></xsl:copy-of></Data>
        </Request>
    </xsl:template>
    
    
    <xsl:template match="//Response" name="response">
          <Response>
          <xsl:attribute name="serviceVer"></xsl:attribute>
          <Header>
         <xsl:attribute name="serviceName"><xsl:value-of select="//header/servicename"/></xsl:attribute>
         <xsl:attribute name="requestName"><xsl:value-of select="//header/requestname"/></xsl:attribute>
         <xsl:attribute name="logLevel"><xsl:value-of select="//header/loglevel"/></xsl:attribute>
         <ClientData><xsl:value-of select="//header/clientdata"></xsl:value-of></ClientData>
         <ClientCallback></ClientCallback>
        </Header> 
        <Data><xsl:copy-of select="//data/*"></xsl:copy-of></Data>
        </Response>
    </xsl:template>
    
</xsl:stylesheet>