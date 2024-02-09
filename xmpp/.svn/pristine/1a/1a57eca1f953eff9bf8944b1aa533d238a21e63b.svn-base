<?xml version="1.0"?>
<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xalan="http://xml.apache.org/xalan" 
    xmlns:ipvs="com.ipvsserv.nxd.xslt.extn.IpvsXsltExtension"
    extension-element-prefixes="ipvs">


    <xalan:component prefix="ipvs" elements="init incr" functions="read">
        <xalan:script lang="javaclass" src="com.ipvsserv.nxd.xslt.extn.IpvsXsltExtension"/>
    </xalan:component>

    <xsl:variable name="serviceDomain" select="//serviceDomain"/>
    <xsl:variable name="usr_list_nid">default.userlist</xsl:variable>
    <xsl:variable name="device_list_nid">default.devicelist</xsl:variable>
    <xsl:variable name="assetresource_list_nid">default.assetresourcelist</xsl:variable>

    <!-- load config xml -->
    <xsl:variable name="config_LoginRequestXml" select="document(concat('/etc/config/','LoginRequest','.xml'))/*"/>
    <xsl:variable name="config_DeviceXml" select="document(concat('/etc/config/','Device','.xml'))/*"/>

    <!-- parse config xml  -->
    <xsl:variable name="user" select="$config_LoginRequestXml//user"/>
    <xsl:variable name="password" select="$config_LoginRequestXml//password"/>
    <xsl:variable name="resource" select="$config_LoginRequestXml//resource"/>
    <xsl:variable name="assetType" select="name($config_DeviceXml)"/>

    <!-- read db -->

    <!-- read db AssetResource-->
    <xsl:variable name="assetResource_whereXpath">Info/Title='<xsl:value-of select="$resource"></xsl:value-of>'</xsl:variable>
    <xsl:variable name="readDB_AssetResource" select="ipvs:readDocument_DB(.,$serviceDomain, $assetResource_whereXpath , $assetresource_list_nid)"/>

    <!-- parse values read from db -->
    <xsl:variable name="db_password" select='$readDB_AssetResource/*/AssetPrivateKey/KeyData'/>
    <xsl:variable name="assetNID" select='$readDB_AssetResource/*/@NID'/>
    <xsl:variable name="db_resource" select='$readDB_AssetResource/*/Info/Title'/>
    <xsl:variable name="db_assetType" select='$readDB_AssetResource/*/Info/Type'/>
  
    <!-- read db Device-->
    <xsl:variable name="devicelistNID" select='ipvs:newParentNID($assetType, $serviceDomain)'/>
    
    <xsl:variable name="device_whereXpath">AssetResourceNID='<xsl:value-of select='$assetNID'/>'</xsl:variable>
    <xsl:variable name="readDB_Device" select="ipvs:readDocument_DB(.,$serviceDomain, $device_whereXpath, $device_list_nid )"/>
    <xsl:variable name="db_assetType_device" select='name($readDB_Device/*)'/>

    <!-- for now no deleting the prev-logged-in user/ resource & device -->
	<!-- read ServiceResource -->
    <xsl:template match="/">
        <DMSRegistartion>
            <!-- debug messages to xml output -->
            <assetResource_whereXpath><xsl:value-of select="$assetResource_whereXpath"></xsl:value-of></assetResource_whereXpath>
            <password><xsl:value-of select="$password"></xsl:value-of></password>
            <resource><xsl:value-of select="$resource"></xsl:value-of></resource>
            <assetType><xsl:value-of select="$assetType"></xsl:value-of></assetType>
            <!-- debug messages to xml output from db parsed-->
            
            <db_assetNID><xsl:value-of select="$assetNID"></xsl:value-of></db_assetNID>
            <db_assetType><xsl:value-of select="$db_assetType"></xsl:value-of></db_assetType>
            <db_resource><xsl:value-of select="$db_resource"></xsl:value-of></db_resource>
            <db_password><xsl:value-of select="$db_password"></xsl:value-of></db_password>
            <device_whereXpath><xsl:value-of select="$device_whereXpath"></xsl:value-of></device_whereXpath>
 
            <xsl:variable name="db_AssetResource_registration">
                <!-- create AssetResource or update AssetResource -->
                <db_AssetResource_registration>
                    <xsl:choose>
                        <xsl:when test="name($readDB_AssetResource/*)='norecords'">
                            <no_records/>    
                            <records>0</records>
                            <xsl:call-template name="addAssetResource"/>                  
                        </xsl:when>
        
                        <xsl:when test="name($readDB_AssetResource/*)='list'">
                            <multiple_records/>
                            <records><xsl:value-of select='count($readDB_AssetResource/*/AssetResource)'></xsl:value-of></records>
                            <xsl:call-template name="deleteAssetResources"/>    
                            <xsl:call-template name="addAssetResource"/>                  
                        </xsl:when>                
        
                        <xsl:when test="name($readDB_AssetResource/*)='AssetResource' and $db_resource !='' ">
                            <one_record/>
                            <xsl:choose>
                                <xsl:when test="not($db_assetType = $assetType) or not($db_password = $password)">
                                    <one_record_diff_type/>
                                    <records>1</records>
                                    <xsl:call-template name="updateAssetResource"/>     
                                </xsl:when>

                                <xsl:otherwise>
                                    <one_record_same_type/>
                                    <records>1</records>
                                    <nid><xsl:value-of select="$readDB_AssetResource/*/@NID"></xsl:value-of></nid>
                                    <result>true</result>                     
                                 </xsl:otherwise>
                            </xsl:choose>
                        </xsl:when>      
                                  
                        <xsl:otherwise>
                             <did_not_match_any_condition/>
                             <records>1</records> 
                             <nid><xsl:value-of select="$readDB_AssetResource/*/@NID"></xsl:value-of></nid>
                             <result>true</result>                     
                       </xsl:otherwise>
                        
                    </xsl:choose>
                </db_AssetResource_registration>
            </xsl:variable>



            <xsl:variable name="db_Device_registration">
                <!-- create Device or update Device -->
                <db_Device_registration>
                
                    <xsl:choose>
                        <xsl:when test="name($readDB_Device/*)='norecords'">
                            <no_records/>    
                            <records>0</records>
                            <xsl:call-template name="addDeviceAndResources"/>                  
                        </xsl:when>
        
                        <xsl:when test="name($readDB_Device/*)='list'">
                            <multiple_records/>
                            <records><xsl:value-of select='count($readDB_Device/*/Device)'></xsl:value-of></records>
                            <xsl:call-template name="deleteDevices"/>    
                            <xsl:call-template name="addDeviceAndResources"/>                  
                        </xsl:when>                
        
                       <xsl:when test="$db_assetType_device != '' ">
                            <one_record/>
                            <xsl:choose>
                                
                                <xsl:when test="$db_assetType_device != $assetType">
                                    <one_record_diff_assetType/>
                                    <db_assetType_device><xsl:value-of select="$db_assetType_device"/></db_assetType_device> 
                                    <on_disc_assetType><xsl:value-of select="$assetType"/></on_disc_assetType> 
                                    <records>1</records>
                                     
                                    <xsl:call-template name="deleteDevice"/>    
                                    <xsl:call-template name="addDeviceAndResources"/>                  
                                </xsl:when> 
                                            
                                <xsl:otherwise>
                                    <one_record_same_type/>
                                    <records>1</records>
                                    <nid><xsl:value-of select="$readDB_Device/*/@NID"></xsl:value-of></nid>
                                    <result>true</result>                     
                                 </xsl:otherwise>
                            </xsl:choose>
                       </xsl:when> 
                        
                       <xsl:otherwise>
                             <did_not_match_any_condition/>
                             <records>1</records> 
                             <nid><xsl:value-of select="$readDB_Device/*/@NID"></xsl:value-of></nid>
                             <result>true</result>                     
                       </xsl:otherwise>
                        
                    </xsl:choose>
                </db_Device_registration>
            </xsl:variable>
			
			<!-- 
            <xsl:copy-of select="$cleanup_prev_login"/>
             -->
            <xsl:copy-of select="$db_AssetResource_registration"/>
            <xsl:copy-of select="$db_Device_registration"/>
 
        </DMSRegistartion>

    </xsl:template>

    <!--  /////////////////////////// assetresource ////////////////////////////////////// -->
    <!--  /////////////////////////// assetresource ////////////////////////////////////// -->
     <xsl:template name="addAssetResource">
        <addingAssetResource> xsl template - add new objectXml_AssetResource</addingAssetResource>
       
        <!-- create ObjectXMLs -->
        <xsl:variable name="objectXml_AssetResource" select="document(concat('/home/onair/objectXml/','AssetResource','.xml'))"/>

        <!-- set AssetResource details -->
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//Title', $resource)"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//Type', $assetType)"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//KeyScheme', 'SERVER_PASSWORD')"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//KeyData', $password)"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setAttributeValue($objectXml_AssetResource/*,'//BotUser','enable', 'true')"/>
         
        <!-- set AssetResource NID -->
        <xsl:variable name="assetNID" select="ipvs:addDocument_DB($objectXml_AssetResource/*,$serviceDomain) "/>
        <xsl:variable name="nid" select="$assetNID"/>
        <nid><xsl:value-of select="$nid"></xsl:value-of></nid>
        <xsl:choose>
            <xsl:when test="string-length($nid)>0 or $nid !=''">
                <result>true</result>
            </xsl:when>
            <xsl:otherwise>
                <result>false</result>
            </xsl:otherwise>
        </xsl:choose>
        
        <!-- todo: remove : add object start    
        <addAssetResource><xsl:copy-of select="$objectXml_AssetResource"></xsl:copy-of></addAssetResource>
        -->
        <!-- todo: remove : add object end -->
         
    </xsl:template>
    
    <xsl:template name="updateAssetResource">
        <updatingAssetResource>xsl template  update AssetResource with password</updatingAssetResource>    
           
        <xsl:variable name="assetNID" select="$readDB_AssetResource/*/@NID"/>
 		<xsl:variable name="dbAssetResourceNID" select="$assetNID"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($readDB_AssetResource/*,'//Type', $assetType)"/>
        <!-- remove keydata or OwnerUserJID-->
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//KeyData', $password)"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setAttributeValue($objectXml_AssetResource/*,'//BotUser','enable', 'true')"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//OwnerUserJID', '')"/>
        <xsl:variable name="objectXml_AssetResource" select="ipvs:setValue($objectXml_AssetResource/*,'//Presence', '')"/>
        <xsl:variable name="db_result" select="ipvs:updateDocument_DB($objectXml_AssetResource/*) "/>
   

        <nid><xsl:value-of select="$dbAssetResourceNID"></xsl:value-of></nid>
        <result><xsl:value-of select="$db_result"></xsl:value-of></result>
        
        <!-- todo: remove : updated object start -->
        <updateAssetResource><xsl:copy-of select="$objectXml_AssetResource"></xsl:copy-of></updateAssetResource>
        
        <!-- todo: remove : updated object start -->
           
    </xsl:template>
    
    <xsl:template name="deleteAssetResources">
        <deletingAssetResources>xsl template - delete AssetResources ... multiple AssetResources found</deletingAssetResources> 
        <xsl:for-each select="$readDB_AssetResource/*/AssetResource/@NID">
            <nid><xsl:value-of select="."></xsl:value-of></nid>
            <xsl:variable name="db_result" select="ipvs:deleteDocumentByNodeID_DB($readDB_AssetResource, .) "/>
            <result><xsl:value-of select="$db_result"></xsl:value-of></result>
        </xsl:for-each>
        
        <!-- todo: remove : deleted objects start 
        <deleteAssetResources><xsl:copy-of select="$readDB_AssetResource"></xsl:copy-of></deleteAssetResources>
        -->
        <!-- todo: remove : deleted objects end -->
        
    </xsl:template>  
    
    
    <!-- /////////////////////////////////////////device//////////////////////////////////////// -->
    <!-- /////////////////////////////////////////device//////////////////////////////////////// -->
     <xsl:template name="updateDevice">
        <updatingDevice>xsl template - update Device with password</updatingDevice>    
            
        <xsl:variable name="dbDeviceNID" select="$readDB_Device/*/@NID"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($readDB_Device/*,'//AssetResourceNID', $assetNID)"/>
        <xsl:variable name="db_result" select="ipvs:updateDocument_DB($objectXml_Device/*) "/>

        <nid><xsl:value-of select="$dbDeviceNID"></xsl:value-of></nid>
        <result><xsl:value-of select="$db_result"></xsl:value-of></result>
        
        <!-- todo: remove : updated object start 
        <updateDevice><xsl:copy-of select="$objectXml_Device"></xsl:copy-of></updateDevice>
        -->
        <!-- todo: remove : updated object start -->
    </xsl:template>
    
    <xsl:template name="deleteDevices">
        <deletingDevices>xsl template - delete Devices ... multiple Devices found</deletingDevices> 
        <xsl:for-each select="$readDB_Device/*/*/@NID">
            <!-- delete device -->
            <nid>delete device : <xsl:value-of select="."></xsl:value-of></nid>
            <xsl:variable name="db_result" select="ipvs:deleteDocumentByNodeID_DB($readDB_Device, .) "/>
            <result><xsl:value-of select="$db_result"></xsl:value-of></result>
            <!-- delete resource -->
            <xsl:for-each select="$readDB_Device/*/*//NIDValue">
                <nid>Delete Resource: <xsl:value-of select="."></xsl:value-of></nid>
                <xsl:variable name="db_result" select="ipvs:deleteDocumentByNodeID_DB($readDB_Device, .) "/>
                <result><xsl:value-of select="$db_result"></xsl:value-of></result>
            </xsl:for-each>
        </xsl:for-each>
        
        <!-- todo: remove : deleted objects start 
        <deleteDevices><xsl:copy-of select="$readDB_Device"></xsl:copy-of></deleteDevices>
        -->
        <!-- todo: remove : deleted objects end -->
    </xsl:template>  

    <xsl:template name="deleteDevice">
        <deletingDevices>xsl template - delete Devices ... Devices type is different </deletingDevices> 
        <xsl:for-each select="$readDB_Device/*/@NID">
            <!-- delete device -->
            <nid>delete device : <xsl:value-of select="."></xsl:value-of></nid>
            <xsl:variable name="db_result" select="ipvs:deleteDocumentByNodeID_DB($readDB_Device, .) "/>
            <result><xsl:value-of select="$db_result"></xsl:value-of></result>
            <!-- delete resource -->
            <xsl:for-each select="$readDB_Device/*//NIDValue">
                <nid>Delete Resource: <xsl:value-of select="."></xsl:value-of></nid>
                <xsl:variable name="db_result" select="ipvs:deleteDocumentByNodeID_DB($readDB_Device, .) "/>
                <result><xsl:value-of select="$db_result"></xsl:value-of></result>
            </xsl:for-each>
        </xsl:for-each>
        
        <!-- todo: remove : deleted objects start
        <deleteDevice><xsl:copy-of select="$readDB_Device"></xsl:copy-of></deleteDevice>
         -->
        <!-- todo: remove : deleted objects end -->
    </xsl:template> 
    <xsl:template name="addDevice">
        <xsl:param name="objectXml_Device" />
        <!-- 
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/XMPPServer").setValue(topic_Get(tApp, "LoginServer"));
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/XMPPServerPort").setValue(topic_Get(tApp, "LoginPort"));
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/User").setValue(OwnerUserName);
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/Password").setValue(OwnerUserPassword);
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/AgentID").setValue(AssetTitle);
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/Domain").setValue(OwnerUserDomain);
            deviceNode.selectSingleNode(".//XMPPAgentServiceConfig/ServiceDomain").setValue(topic_Get(tApp, "ServiceDomain"));
        -->
        
        <xsl:variable name="xmppServer" select="/ipvs/loginServer"/>
        <xsl:variable name="xmppServerPort" select="/ipvs/loginPort"/>

        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/XMPPServer', $xmppServer)"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/XMPPServerPort', $xmppServerPort)"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/User', $user)"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/Password', $password)"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/AgentID', $resource)"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/Domain', 'string')"/>
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//XMPPAgentServiceConfig/ServiceDomain', $serviceDomain)"/>
        <!-- 
            var childInstanceListNode = deviceNode.selectSingleNode(".//StreamingServiceStatus/InstanceList");
            if( childInstanceListNode != null ) {
            childInstanceListNode.removeChildren();
            }
         -->
        <xsl:variable name="objectXml_Device" select="ipvs:deleteChildren($objectXml_Device/*,'//StreamingServiceStatus/InstanceList')"/>

         
        <!-- set Device NID -->
        <xsl:variable name="nid" select="ipvs:addDocument_DB($objectXml_Device/*,$serviceDomain) "/>
        <nid><xsl:value-of select="$nid"></xsl:value-of></nid>
        <xsl:choose>
            <xsl:when test="string-length($nid)>0 or $nid !=''">
                <result>true</result>
            </xsl:when>
            <xsl:otherwise>
                <result>false</result>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- ////////////////////// addResourceDevice start ////////////-->
    <xsl:template name="addResourceDevice">
        <xsl:param name="assetNID" />
        <xsl:param name="resource_name" />
        <xsl:param name="resource_index" />
        <xsl:param name="streamTypeList" />
        <xsl:param name="maxInstanceCount" />
        <xsl:param name="resourceGroups" />    
      
 
        <xsl:variable name="objectXml_resource" select="document(concat('/home/onair/objectXml/',$resource_name,'.xml'))"/>
        <xsl:variable name="objectXml_resource" select="ipvs:setValue($objectXml_resource/*,'//AssetResourceNID', $assetNID)"/>
        <xsl:variable name="objectXml_resource" select="ipvs:setValue($objectXml_resource/*,'//Info/Title', $resource)"/>
        <xsl:variable name="objectXml_resource" select="ipvs:setValue($objectXml_resource/*,'//InstanceInfo/MaxInstanceCount', $maxInstanceCount)"/>
        <xsl:variable name="objectXml_resource" select="ipvs:setValue($objectXml_resource/*,'//Info/DeviceElementID', $resource_index)"/>
        
        <!-- delete Tags and add more -->
        <xsl:variable name="objectXml_resource" select="ipvs:deleteElement($objectXml_resource/*,'//Groups')"/>
        <xsl:variable name="objectXml_resource" select="ipvs:appendChildElement($objectXml_resource/*,'//Info', $resourceGroups) "/>  
     
        <!-- delete StreamType and add more -->
        <xsl:variable name="objectXml_resource" select="ipvs:deleteElement($objectXml_resource/*,'//StreamTypeList/StreamType')"/>
        <xsl:variable name="objectXml_resource" select="ipvs:addChildElements($objectXml_resource/*,'//StreamTypeList' , 'StreamType', $streamTypeList)"/>

        <!-- set Device NID -->
        <xsl:variable name="objectXml_resource" select="ipvs:setValue($objectXml_resource/*,'//StreamTypeList/StreamType', 'V2D')"/>
        <xsl:variable name="nid" select="ipvs:addDocument_DB($objectXml_resource/*,$serviceDomain) "/>
        <xsl:value-of select="$nid"></xsl:value-of>

    </xsl:template>
    <!-- ////////////////////// addResourceDevice end ////////////-->

    <xsl:template name="addDeviceAndResources">
        <xsl:variable name="readDB_AssetResource" select="ipvs:readDocument_DB(.,$serviceDomain, $assetResource_whereXpath, $assetresource_list_nid )"/>
        <xsl:variable name="assetNID" select='$readDB_AssetResource/*/@NID'/>
    
        <addingDevice> xsl template - add new objectXml_Device <xsl:value-of select="$assetNID"></xsl:value-of></addingDevice>
        <!-- create ObjectXMLs -->
        <xsl:variable name="objectXml_Device" select="document(concat('/home/onair/objectXml/',$assetType,'.xml'))"/>

        <!-- set Device details -->
        <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//AssetResourceNID', $assetNID)"/>
        
        <!-- todo: remove : add object end -->
        <xsl:choose>

            <xsl:when test="$assetType='V2D-Tx-Device'">
                 <!-- do noting ??? -->
            </xsl:when>

            <xsl:when test="$assetType='V2D-Rx-Device'">
                 <!-- do noting ??? -->
            </xsl:when>

            <xsl:when test="$assetType='V2D-XP200-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='MPEGTS-Tx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='RTP-Tx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='UDP-Tx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='MPEGTS-Rx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='RTP-Rx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='UDP-Rx-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='IPVSClient-Device'">
				<!-- do noting ??? -->                
            </xsl:when>

            <xsl:when test="$assetType='DMS-100-Device' or $assetType='DMS-200-Device' or $assetType='DMS-1000-Device'
             or $assetType='DMS-1200-Device' or $assetType='DMS-1500-Device' or $assetType='DMS-25-Device' or $assetType='DMS-1550-Device' or $assetType='DMS-150-Device' or $assetType='SG-100-Device'" >
                <!-- 
                      StreamTypeArr = ["V2D","MPEGTS","RTP","UDP"];
                      resourceMapRuleArr.push({objtype:"MediaStorePortResource", rule:"MediaStorePortResource", 
                                 defaultNID:"DEFAULT_MEDIASTOREPORTRESOURCE_NID", 
                                 StreamType:StreamTypeArr, state:false},
                                {objtype:"MediaStreamRelayPortResource", rule:"MediaStreamRelayPortResource", 
                                 defaultNID:"DEFAULT_MEDIASTREAMRELAYPORTRESOURCE_NID", 
                                 StreamType:StreamTypeArr, state:false}
                -->
                <!-- save resource -->   
                <xsl:variable name="resource_nid"><xsl:call-template name="addResourceDevice">
                    <xsl:with-param name="resource_name">MediaStorePortResource</xsl:with-param>
                    <xsl:with-param name="assetNID" select='$assetNID'/>
                    <xsl:with-param name="resource_index">1</xsl:with-param>
                    <xsl:with-param name="streamTypeList">V2D,MPEGTS,RTP,UDP</xsl:with-param>
                    <xsl:with-param name="maxInstanceCount">25</xsl:with-param>
                    <xsl:with-param name="resourceGroups" select='$readDB_AssetResource/*/Info/Groups'/>
                </xsl:call-template></xsl:variable>
                
                <!-- added resource -->
                <resource_nid>MediaStorePortResource <xsl:value-of select='$resource_nid'/></resource_nid>
                <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//ResourcePointers/MediaStorePortResourceNID/NIDValue', $resource_nid)"/>
                        
                        
                 <!-- save resource 2 -->   
                <xsl:variable name="resource_nid"><xsl:call-template name="addResourceDevice">
                    <xsl:with-param name="resource_name">MediaStreamRelayPortResource</xsl:with-param>
                    <xsl:with-param name="assetNID" select='$assetNID'/>
                    <xsl:with-param name="resource_index">1</xsl:with-param>
                    <xsl:with-param name="streamTypeList">V2D,MPEGTS,RTP,UDP</xsl:with-param>
                    <xsl:with-param name="maxInstanceCount">25</xsl:with-param>
                    <xsl:with-param name="resourceGroups" select='$readDB_AssetResource/*/Info/Groups'/>
                </xsl:call-template></xsl:variable>
                
                <!-- added resource -->
                <resource_nid>MediaStreamRelayPortResource <xsl:value-of select='$resource_nid'/></resource_nid>
                <xsl:variable name="objectXml_Device" select="ipvs:setValue($objectXml_Device/*,'//ResourcePointers/MediaStreamRelayPortResourceNID/NIDValue', $resource_nid)"/>
                                                
                <!-- save device -->   
                <xsl:call-template name="addDevice">
                    <xsl:with-param name="objectXml_Device" select="$objectXml_Device" />
                </xsl:call-template>                
            </xsl:when>
           

            <xsl:when test="$assetType='MS-Device' or $assetType='MS-25-Device'">
                
                <!-- save device -->   
                <xsl:call-template name="addDevice">
                    <xsl:with-param name="objectXml_Device" select="$objectXml_Device" />
                </xsl:call-template> 
               
            </xsl:when>
            
            <xsl:when test="$assetType='Template'">
               <!-- do noting ??? -->
            </xsl:when>
            
            <xsl:otherwise>
                 <!-- do noting ??? -->
            </xsl:otherwise>
        </xsl:choose>
        


    </xsl:template>
    
 
    
</xsl:stylesheet>
