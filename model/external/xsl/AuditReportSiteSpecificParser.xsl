<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" indent="no"/>

  <!-- Please edit the select="" attribute to enter the name of the site for report generation --> 
  
  <xsl:param name="sitename"  select="'HA'" required="yes"/>
  
  <xsl:template match="/">
  	<xsl:apply-templates select="//Log[@type='custom']"/>  	
  	<xsl:text> </xsl:text>
  </xsl:template>     
  
  <xsl:template match="Log">	                       		
	<!-- Name of the site is followed by a space -->           		
	<xsl:if test="./Description/EventXML/EventUser/User/Groups/Tag[@Name='Location']/@Value = $sitename">               
	      <xsl:value-of select="@date" />         
          
          <xsl:text>, </xsl:text>                                
          <xsl:value-of select="@time" />
          
          <xsl:text>, </xsl:text>          
          <xsl:value-of select="@name" />
          
          <xsl:text>, </xsl:text>          
          <xsl:value-of select="@user" />
          
          <xsl:text>, </xsl:text>          
          <xsl:value-of select="./Description/EventXML/EventAsset/Asset/@title" />
          
          <xsl:text>, </xsl:text>            
          <xsl:value-of select="./Description/EventXML/EventUser/User/Groups/Tag[@Name='Location']/@Value" />
                    
          <xsl:if test="contains(@name,'Searched')">
          	<xsl:text>, </xsl:text>
		    <xsl:value-of select="./Description/@text"/>
		  </xsl:if>
          
		  <xsl:if test="contains(@name,'Modified') or contains(@name,'Viewed') or contains(@name,'Deleted')">
		  	<xsl:text>, </xsl:text>
		  	
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='ID']" />				
			<xsl:text>&gt;</xsl:text>
			
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Hospital']" />							
			<xsl:text>&gt;</xsl:text>
			
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Lab']" />				
			<xsl:text>&gt;</xsl:text>
			
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Physician']" />				
			<xsl:text>&gt;</xsl:text>
			
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Patient']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='PatientID']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='PatientDOB']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='PatientSex']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="substring-before(./Description/EventXML/MetaData/Tag[@Name='Date'],' ')"/>
			<xsl:text>&gt;</xsl:text>

			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="substring-after(./Description/EventXML/MetaData/Tag[@Name='Date'],' ')"/>
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Length']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='ProcedureType']" />				
			<xsl:text>&gt;</xsl:text>
				
			<xsl:text>&lt;</xsl:text>
			<xsl:value-of select="./Description/EventXML/MetaData/Tag[@Name='Notes']" />				
			<xsl:text>&gt;</xsl:text>
		  </xsl:if>
			                        
          <xsl:text>&#xa;</xsl:text>                                     
  	</xsl:if>	
  </xsl:template>
  
</xsl:stylesheet>
