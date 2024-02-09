package com.ipvs.device.StreamAgents;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.systemservice.handler.DownloadDBRequestHandler;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.DBUpdateEvent;


public class DeviceConfigStreamAgent extends ConfigStreamAgent {
	private String CONFIG_HANDLER_XML = "/com/ipvs/xsd/xml/confighandler.xml";
	private Document  configHandlerDoc = null;
	
	private enum UPDATE_CHILD_XPATH {
		IPVSDbService("//XMPPServiceConfig/IPVSDBService"),
		HAServer("//ServiceElementsConfig/HAServiceConfig/HAServer"),
		XMPPServer("//XMPPServiceConfig/XMPPServer"),
		MaxMediaClients("//ServicePreferences/Login");
		
		private String value;
		
		private UPDATE_CHILD_XPATH(String value) {
			this.value = value;
		}
		
		public String toString() {
			return this.value;
		}
	};
	
    public DeviceConfigStreamAgent(String resourceTitle, String localConfigFile,Document agentInitTasksDoc) {
        setTitle(resourceTitle);
        setConfigFile(localConfigFile);
        this.agentInitTaskDoc = agentInitTasksDoc;
    }

    public boolean handleEvent(MRClient client, Document doc, Element eventElement)
        throws Exception {
        try {

        	if(configHandlerDoc == null) {
        		configHandlerDoc = MRXMLUtils.loadXMLResource(CONFIG_HANDLER_XML, log);
        	}
            if ((doc == null) || (MRXMLUtils.getRootElement(doc) == null)) {
                return false;
            }
              
            // if we are waiting for active server device NID and we got service resource update find active server nid and compare update if mismatches
            if(this.activeDeviceNIDDiscoveryState.equals(WAIT_FOR_ACTIVE_SERVER_DISCOVERY) &&  
        			AppServerAgentUtils.isServiceResourceUpdateEvent(doc.asXML(), log, db, this.userJID,this.xf.sf(userJID))){            	
             	compareAndUpdateStandByServerConfiguration();
            	return false;
        	}
            
            DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(doc);

            if(dbUpdateEvent.getUpdateEventElement() !=null){
                String NID = dbUpdateEvent.getSubscribedNID();
                
                //if we are watching device updates of active server and we got notification for active server 
                //then compare and update config if mismatches 
                String updateNID = dbUpdateEvent.getUpdateNID();
                 
                if(this.activeDeviceNIDDiscoveryState.equals(WATCH_FOR_ACTIVE_SERVER_CONFIG_UPDATE) && this.activeDeviceNID.equals(updateNID)){
                   String childXpath = Utils.decode(dbUpdateEvent.getChildXPath());
                   log.addInfo("Debug log for bug 26661 childXpath :="+childXpath);
                   boolean compareAndUpdate = false;
                   for (UPDATE_CHILD_XPATH xpath : UPDATE_CHILD_XPATH.values()) {
                	   if(childXpath.equals(xpath.toString())) {
                		   compareAndUpdate = true;
                		   break;
                	   }
                   }
                   if(compareAndUpdate) {
                	      log.addInfo("Got Active Device Configuration Update. Compare and Update stand by server configuration");
                          compareAndUpdateStandByServerConfiguration();	
                   }
                   return false;
                }    

                if ((deviceConfigNID != null) && NID.trim().equals(deviceConfigNIDData)) {

                    if(dbUpdateEvent.isAddorUpdate()){
                   
                        String requestNID = "ConfigStreamAgentRequestNID";
                        MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, requestNID, "serviceName", "requestName", new MRMessage(getRequestDoc()), "");
                        Document updatedoc = null;
                        
                        updatedoc = MRXMLDBUtils.readElement(request, log, db, deviceConfigNID);                        

                        if (updatedoc != null) {
                            Document docModify = ConfigDeviceUtils.findDirtyChildren(updatedoc, log);
                            List<?> list = MRXMLUtils.getList(docModify.getRootElement());

                            if ((list == null) || (list.size() == 0)) {
                                return false;
                            }

                            //System.out.println("ConfigStreamAgent: calling handleConfig= " + MRXMLUtils.documentToString(docModify));
                            ConfigDeviceUtils.handleConfig(docModify, CONFIG_HANDLER_XML, log);

                            return true;
                        }                       
                    }
                }
            }
        } catch (Exception ex) {
        }

        return false;
    }

	//Update the device Config XML
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc) throws Exception {    	
        initInternal(xf, log, db, resourceFactory, userJID,  setServiceDomainResponseDoc);
        //update api key in the local file system
        updateAPIKey(log,setServiceDomainResponseDoc);
        
        if(configHandlerDoc == null) {
    		configHandlerDoc = MRXMLUtils.loadXMLResource(CONFIG_HANDLER_XML, log);
    	}
        if (deviceNID != null) {
            this.deviceNID = deviceNID.trim();
            this.deviceConfigNID = this.deviceNID + ".DeviceConfig";
            this.deviceConfigNIDData = this.deviceNID + ".DeviceConfig/data";
        }

        if ((deviceNID != null) && (deviceNID.trim().length() != 0)) {
            // This should check the local Device config
            // If the local file has device has a local override flag then it  will
            // replace the config on the server with the new local config
            initializeDeviceConfigInDB(localConfigFile, deviceNID, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID);
            sendPortResourceTypeRequestToAppServer(localConfigFile, deviceNID, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID);
            //initializeDeviceStatusInDB(localConfigFile, deviceNID, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
            DeviceAppUtils.publishStatus(localConfigFile, log);


            try {
                MRXMLDBUtils.subscribe(MRRequest.createRequest(userJID), db, log, deviceNID, "", "");

                //xmpp.PubSub_Subscribe(deviceNID);
            } catch (Exception e) {
                e.printStackTrace();
            }

           this.watchList.put(deviceNID, Integer.valueOf(0));
        }
        
        compareAndUpdateStandByServerConfiguration();
       
     	String requestName = MRRequest.getRequestName(DownloadDBRequestHandler.class.getName());        
        String requestToken = AgentInitUtils.getRequestTokenForGivenTaskName(this.agentInitTaskDoc, requestName);
        HAUtils.handleHA(log, db, xf.xmpp(userJID), xf.sf(userJID), userJID, deviceType,setServiceDomainResponseDoc, requestToken);
        if(HAUtils.isHAServiceEnabled() && HAUtils.isServerInStandby()) {
        	AgentInitUtils.updateAgentStateTask(agentInitTaskDoc, requestName, AgentInitUtils.AGENT_STATE_WAITING_FOR_RESPONSE_FROM_APPSERVER);
        }
    }
    
    public void initInternalReliability(MRLog log, Document setServiceDomainResponseDoc) throws Exception {
    	// re-write the APIKeyTable on every init.
    	updateAPIKey(log, setServiceDomainResponseDoc);
    }
    
    private void updateAPIKey(MRLog log, Document setServiceDomainResponseDoc) throws Exception {
		Element APIKeyTable = MRXMLUtils.getElementXpath(setServiceDomainResponseDoc, "//ApiKeyTable");
		FileUtils.writeFile(DeviceConst.API_KEY_TABLE_FILE_PATH, MRXMLUtils.elementToString(APIKeyTable));
		log.addInfo("ConfigStreamAgent: Updated APIKeyTable in "+DeviceConst.API_KEY_TABLE_FILE_PATH,MRLog.INFO,MRLog.NOTIFY);
	}

	//HA Synch up constants
    private String HA_CONFIG_CLI_COMMAND = "set_ha";
    private enum HA_USER_CONFIGURABLE_VALUES {HAServerEnable,PeerIP,Primary,PeerName};
    private String WAIT_FOR_ACTIVE_SERVER_DISCOVERY = "WaitForActiveServerDiscovery";
    private String WATCH_FOR_ACTIVE_SERVER_CONFIG_UPDATE = "WatchForActiveServerConfigUpdate";

    /**
     * Find the active server resource NID  
     * @return active server asset resource NID
     * @throws MRException
     * @throws Exception
     */
    private String getActiveServerAssetResourceNID() throws MRException, Exception {

    	String serviceResourceNID = AppServerAgent.getMsAssetNID();
    	Element haInfoEL = AssetAdminServiceUtils.getChild(serviceResourceNID, "//HA", MRRequest.createRequest(this.userJID), log, db, xf.xmpp(userJID), resourceFactory, xf.sf(userJID));
    	String activeResourceNID = MRXMLUtils.getAttributeValueXpath(MRXMLUtils.getElement(haInfoEL, "HA"), "./*[@state='active']", "MSAssetResourceNID", "");
    	if(activeResourceNID.isEmpty()) {
    		this.activeDeviceNIDDiscoveryState = WAIT_FOR_ACTIVE_SERVER_DISCOVERY;    		
    	} else {
    		this.activeDeviceNIDDiscoveryState = WATCH_FOR_ACTIVE_SERVER_CONFIG_UPDATE;   
    	}
    	log.addInfo("set ActiveDeviceNIDDiscoveryState :="+this.activeDeviceNIDDiscoveryState);
    	return activeResourceNID;
    }

    /**
     * Compare Stand by DB, HA, and LDAP configuration with active server
     * If mismatches replace the stand by configuration with active configuration
     * @throws MRException
     * @throws Exception
     */
    private void compareAndUpdateStandByServerConfiguration() throws MRException, Exception {
    	// If the server is in stand by then no need to compare configuration.
    	if(!HAUtils.isServerInStandby()) {
    		return;
    	}
    	
		String selectXML = "<Device>"
				+ "{root($VAR)/*/@NID}"
				+ "<ServiceElementsConfig>{$VAR//HAServiceConfig}</ServiceElementsConfig>" 
				+ "<ServiceConfig>{$VAR//XMPPServiceConfig}</ServiceConfig>"
				+ "{$VAR//EthernetPortTableStatus}</Device>";

    	String whereXPath = "";
    	Element activeServerDeviceEl =  null;

    	// if active device nid not found find the nid
    	if(this.activeDeviceNID.isEmpty()) {    	
    		String activeResourceNID = getActiveServerAssetResourceNID();
    		if(this.activeDeviceNIDDiscoveryState.equals(WAIT_FOR_ACTIVE_SERVER_DISCOVERY)){
    			return;
    		}
    		whereXPath = "AssetResourceNID[.='"+activeResourceNID+"']";
    		activeServerDeviceEl = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device, selectXML, whereXPath, MRRequest.createRequest(this.userJID), db, log, xf.xmpp(userJID));
    		this.activeDeviceNID = MRXMLUtils.getAttributeValueXpath(activeServerDeviceEl,"./Device","NID","");
    		log.addInfo("Set ActiveDeviceNID :="+activeDeviceNID);
    		MRXMLDBUtils.subscribe(MRRequest.createRequest(userJID), db, log, activeDeviceNID, "", "");
    	}  else {
    		activeServerDeviceEl = AssetAdminServiceUtils.getChildBasedOnSelectXMLNID(this.activeDeviceNID, selectXML, whereXPath, MRRequest.createRequest(this.userJID), db, log, xf.xmpp(userJID));            
    	}


    	Document activeDeviceDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElement(activeServerDeviceEl, "Device"));
    	Document standByDeviceDoc = MRXMLUtils.loadXMLFile(localConfigFile, log);

       	//compare HA configuration and update if mismatches
    	boolean isHAReplaced = compareAndUpdateHAServiceConfiguration(activeDeviceDoc,standByDeviceDoc);

    	//compare service configuration and update if mismatches
    	boolean isServiceConfigReplaced = compareAndUpdateServiceConfiguration(activeDeviceDoc,standByDeviceDoc);
    	
    	if(isHAReplaced || isServiceConfigReplaced) {
    		DeviceAppUtils.saveToFlash(log);
    	}

    }

    /**
     * Compare Ha configuration if mismatches then replace
     * @param activeDeviceDoc
     * @param standByDeviceDoc
     * @throws MRException
     * @throws Exception
     */
    private boolean compareAndUpdateHAServiceConfiguration(Document activeDeviceDoc,
    		Document standByDeviceDoc) throws MRException, Exception {
    	Element activerServerHAConfigEL = MRXMLUtils.getElementXpath(activeDeviceDoc, "//HAServiceConfig");
    	if(activerServerHAConfigEL == null) {
    		return false;
    	}


    	Document activeServerHAconfigDoc = MRXMLUtils.elementToDocument(activerServerHAConfigEL);

    	Document standByServerHAconfigDoc = MRXMLUtils.getDocumentXpath(standByDeviceDoc, "//HAServiceConfig");

    	//Check HA Configuration    	
    	List<String> misMatchConfigList = compareConfigElements(standByServerHAconfigDoc, activeServerHAconfigDoc, HA_CONFIG_CLI_COMMAND);
    	boolean updateHAConfig = false;

    	if(!misMatchConfigList.isEmpty()) {
    		Iterator<String> misMatchIterator = misMatchConfigList.iterator();
    		while(misMatchIterator.hasNext()) {
    			String misMatchParam = misMatchIterator.next();
    			boolean ignoreMatch = false;
    			for(HA_USER_CONFIGURABLE_VALUES userConfigurableParam: HA_USER_CONFIGURABLE_VALUES.values()) {
    				if(misMatchParam.equals(userConfigurableParam.name())){
    					ignoreMatch = true;
    					break;
    				}
    			}

    			if(!ignoreMatch) {
    				MRXMLUtils.setValueXpath(standByServerHAconfigDoc, "//"+misMatchParam, MRXMLUtils.getValueXpath(activeServerHAconfigDoc, "//"+misMatchParam, ""));
    				updateHAConfig = true;
    			}

    		}

    	}

    	String activeServerPeerIP = MRXMLUtils.getValueXpath(standByServerHAconfigDoc, "//HAServer/PeerIP", "");
    	String activeServerPeerName = MRXMLUtils.getValueXpath(standByServerHAconfigDoc, "//HAServer/PeerName", "");
    	Element parameterEl = MRXMLUtils.getElementXpath(activeDeviceDoc, "//EthernetPortTableStatus/EthernetPortStatus/Parameters[Parameter[@name='IPAddress' or .='"+activeServerPeerIP+"']]");    	
    	String activeServerName = MRXMLUtils.getValueXpath(parameterEl, "./Parameter[@name='Name']", "");

    	if(!activeServerPeerName.isEmpty() && !activeServerPeerName.equals(activeServerName)){
    		MRXMLUtils.setValueXpath(standByServerHAconfigDoc, "//HAServer/PeerName", activeServerName);
    		updateHAConfig = true;
    	}


    	if(updateHAConfig) {
    		log.addInfo("Active and Stand By server HA Service Configuration Not matched replacing stand by server configuration with active server configuration");
    		Element serviceElementConfig=MRXMLUtils.newElement("ServiceElementsConfig");
    		MRXMLUtils.addElement(serviceElementConfig,(Element)standByServerHAconfigDoc.getRootElement().detach());
    		updateStandByServerConfig((Element)serviceElementConfig.detach());
    	}

    	return updateHAConfig;
    }

        
    /**
     * Compares Service element configuration if mismatches then replace 
     * @param activeDeviceDoc
     * @param standByDeviceDoc
     * @throws Exception
     */
    private boolean  compareAndUpdateServiceConfiguration(Document activeDeviceDoc,
    		Document standByDeviceDoc) throws Exception {
    	Element activeServerDBconfigEl = MRXMLUtils.getElementXpath(activeDeviceDoc, "//XMPPServiceConfig");
    	
    	List<String> misMatchConfigList =  null;
    	List<String> deviceMismatchConfigList =  new ArrayList<String> ();
    	
    	if(activeServerDBconfigEl != null) {

    		Element standByServerDBconfigEl = MRXMLUtils.getElementXpath(standByDeviceDoc, "//XMPPServiceConfig");

    		 for (UPDATE_CHILD_XPATH xpath : UPDATE_CHILD_XPATH.values()) {
    			 Element configHandlerEl = MRXMLUtils.getElementXpath(configHandlerDoc, "//handler[triggername='" + xpath.toString() + "']");
    			 if(configHandlerEl != null) {
    				 String cmdName = MRXMLUtils.getValueXpath(configHandlerEl, "cmd", "");
    				 misMatchConfigList = compareConfigElements(MRXMLUtils.elementToDocument(standByServerDBconfigEl), MRXMLUtils.elementToDocument(activeServerDBconfigEl), cmdName);
    				 deviceMismatchConfigList.addAll(misMatchConfigList);
    			 }
    		 }
    
    		if(!deviceMismatchConfigList.isEmpty()) {
    			log.addInfo("Active and Stand By server ServiceElement Configuration Not matched replacing stand by server configuration with active server configuration");
    			updateStandByServerConfig(MRXMLUtils.getElementXpath(activeDeviceDoc, "//ServiceConfig/XMPPServiceConfig"));
    			return true;
    		}
    	}

    	return false;
    }
    
    
    /**
     * Update configuration by running cli commands on box
     * @param serviceConfigEl
     * @throws Exception
     */
    private void updateStandByServerConfig(Element serviceConfigEl) throws Exception {
    	Element configEL = MRXMLUtils.newElement("DeviceConfig");
    	MRXMLUtils.addElement(configEL,(Element)serviceConfigEl.detach());
    	ConfigDeviceUtils.handleConfig(MRXMLUtils.elementToDocument(configEL),CONFIG_HANDLER_XML , log);
    }

    /**
     * Compares the configurable parameter specified in confighandler.xml for gicven cli command
     * @param standByServerConfigEl
     * @param activeServerConfigEl
     * @param command
     * @return list of mismatched element names
     * @throws MRException
     * @throws Exception
     */
    private List<String> compareConfigElements(Document standByServerConfigEl, Document activeServerConfigEl,String command) throws MRException,Exception {
    	
    	Element dbConfigCommandEL = MRXMLUtils.getElementXpath(configHandlerDoc, "//handler[cmd='"+command+"']");
    	List<String> missMatchElementNameList = new ArrayList<String>();

    	List<?> paramEntry =  MRXMLUtils.getListXpath(dbConfigCommandEL, ".//paramentry");

    	if(paramEntry == null || paramEntry.isEmpty()) {
    		return missMatchElementNameList;
    	}
    	
    	for(Object paramEntryObject : paramEntry) {
    		String paramVal = MRXMLUtils.getValueXpath((Element)paramEntryObject, "./paramval", "");
    		String xpath = "//"+paramVal;    	
    		String activeServerDbConfigValue = MRXMLUtils.getValueXpath(activeServerConfigEl, xpath, "");
    		String standByServerDbConfigValue = MRXMLUtils.getValueXpath(standByServerConfigEl, xpath, "");
    		if(!activeServerDbConfigValue.equals(standByServerDbConfigValue)){
    			missMatchElementNameList.add(paramVal);
    		}
    	}

    	return missMatchElementNameList;
    }
    
    
}
