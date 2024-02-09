package com.ipvs.mediaroomservice.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.assetadminservice.device.handler.SetServiceClientStatusRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.port.handler.SetPortResourceTypeRequestHandler;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.handler.GetMediaRoomInvitesRequestHandler;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;


public class ConfigStreamAgent implements ServiceAgentI, ConfigListenerI {
    protected String title;
    protected String localConfigFile;
    protected String deviceNID = null;
    protected String deviceConfigNID = null;
    protected String deviceConfigNIDData = null;
    protected MRLog log;
    protected Hashtable<String, Integer> watchList;
    protected XMPPF xf;
    protected MRResourceFactoryI resourceFactory;
    protected String userJID;
    protected MRDB db;
    protected String deviceType = null;
    protected List<String> resourceNIDList = new ArrayList<String>();
    protected String activeDeviceNIDDiscoveryState = "";
    protected String activeDeviceNID = "";
    protected Document agentInitTaskDoc  = null;
  
    public void addResourceToList(String resourceType, String NID) {
        if (NID == null) {
            return;
        }

        resourceNIDList.add(NID);
    }

    public String getDeviceType() {
        return deviceType;
    }

    public MRConfig getConfig(String serviceName, String objectType, MRLog log) {
        try {
            return MRConfig.getInstance(serviceName, objectType, log);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    protected void setConfigFile(String localConfigFile) {
        this.localConfigFile = localConfigFile;
    }

    protected void setTitle(String title) {
        this.title = title;
    }

    public String getTitle() {
        return this.title;
    }

    protected Document getRequestDoc() {
        Document redoc = null;

        try {
            redoc = MRXMLUtils.stringToDocument("<Request><Header></Header><data></data></Request>");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return redoc;
    }

    public boolean handleEvent(MRClient client, Document doc, Element eventElement)
        throws Exception {
        return false;
    }

    public String handleMessage(MRClient client, String fromJID, String message)
    throws Exception {
    	Document messageDoc = MRXMLUtils.stringToDocument(message);
    	String messagetype = MRXMLUtils.getAttributeValueXpath(messageDoc, "//AgentMessage","type", "");
    	if (ServiceClient.RESPONSE.equals(messagetype)) {
    		this.handleResponse(messageDoc);
    	}  else if (ServiceClient.EVENT.equals(messagetype)) {
            this.handleAgentEvent(client, messageDoc, fromJID);
        } else if(ServiceClient.REQUEST.equals(messagetype)){
        	this.handleRequest(messageDoc);
        }
    	return message;
    }

    private void handleRequest(Document messageDoc) throws MRException,Exception{
    	String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		if(requestName.equals(HAUtils.ON_OFF_HA_SERVER)){			
			HAUtils.handleOnOffHAServer(messageDoc, log);
		}
	}

	private void handleResponse(Document messageDoc) throws MRException,Exception{
    	String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		String strState = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH, MRConst.STATE, "");
		log.addInfo("ConfigStreamAgent : Recieved Response From Appserver RequestName:="+requestName +" State:="+strState,MRLog.INFO,MRLog.NOTIFY);

		if(requestName.equals("DownloadDBRequest")){
			try {				
				HAUtils.handleDownloadDBResponse(messageDoc, log, xf.xmpp(userJID), userJID, xf.sf(userJID));
				this.handleAgentInitRequestResponse(messageDoc, strState);
			} catch(MRException exp){
				exp.printStackTrace();
			    //updateResourceStateAsError();
			} catch(Exception exp){
				exp.printStackTrace();
		       //updateResourceStateAsError();				
			}
		} else if(requestName.equals(MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName())) ||
				requestName.equals(MRRequest.getRequestName(SetServiceClientStatusRequestHandler.class.getName())) || 
				requestName.equals(MRRequest.getRequestName(SetPortResourceTypeRequestHandler.class.getName()))) {
			this.handleAgentInitRequestResponse(messageDoc, strState);
		}
	}
	
	
	private void handleAgentInitRequestResponse(Document messageDoc,String strState){
		try {
			if(this.agentInitTaskDoc==null) {
				return;
			}
			
			String requestToken = MRXMLUtils.getValueXpath(messageDoc, "//"+MRConst.CLIENT_DATA,"");
			int state = -1;
			if(strState.equals(""+MRRequest.STATE_DONE)) {
				state=AgentInitUtils.AGENT_STATE_DONE;					
			} else if(strState.equals(""+MRRequest.STATE_EXCEPTION)) {
				state=AgentInitUtils.AGENT_STATE_ERROR;
			}
			
			updateAgentTaskState(requestToken, state);
			
		}  catch (Exception e) {			
			e.printStackTrace();
		}
	}
	
	protected void updateAgentTaskState(String requestToken,int state) throws MRException,Exception {
		if(this.agentInitTaskDoc == null) {
			return;
		}
		
		AgentInitUtils.updateAgentStateForGivenRequestToken(this.agentInitTaskDoc, requestToken, state);	
		Element taskEl = MRXMLUtils.getElementXpath(this.agentInitTaskDoc, "//Task[@state!='"+AgentInitUtils.AGENT_STATE_DONE+"']");
	    if(taskEl != null) {
			return;
		}
		this.agentInitTaskDoc = null;
		
		this.sendAgentStateEventToMRClientAgent();
	}
	
	
	private void handleAgentEvent(MRClient client, Document messageDoc,
			String fromJID) throws MRException,Exception {
    	
    	String action = MRXMLUtils.getValueXpath(messageDoc, "//" + DeviceConst.ACTION, "");
        if (action.equals(MediaRoomServiceConst.SYSTEMEVENT)) {
            handleSystemEvent(client, messageDoc, fromJID);
        }
	}

	private void handleSystemEvent(MRClient client, Document systemDoc,
			String fromJID) throws MRException,Exception {
		log.addInfo("ConfigStreamAgent: Handle System Event");
	    String eventSource = MRXMLUtils.getValueXpath(systemDoc, "//EventSource", "");
        String eventType = MRXMLUtils.getValueXpath(systemDoc, "//EventType", "");
        if (eventSource.isEmpty() || eventType.isEmpty()) {
            return;
        }
        
        if (eventSource.equals("HA") && eventType.equals("OnOffSecondary")) {
        	Document devicedoc = MRXMLUtils.stringToDocument(FileUtils.readFile(this.localConfigFile));
        	String action  = MRXMLUtils.getValueXpath(systemDoc, "//Action", "");
        	HAUtils.handleHAEvents(userJID,log,db,xf.xmpp(userJID),xf.sf(userJID),resourceFactory,devicedoc,action);        	
        }
		
	}


    //Update the device Config XML and update the contact JID of logged in asset resource,Device and ports.
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc)
        throws Exception {
        initInternal(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
        this.sendAgentStateEventToMRClientAgent();
    }

    protected void initInternal(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc)
        throws Exception {
        this.xf = xf;
        this.log = log;
        this.log.setLogLevel(MRLog.INFO);
        this.resourceFactory = resourceFactory;
        this.userJID = userJID;
        this.watchList = new Hashtable<String, Integer>();
        this.db = db;

        try {
            this.deviceNID = MRXMLUtils.getValueXpath(setServiceDomainResponseDoc, "//deviceNID", "");
        } catch (Exception e) {
            this.deviceNID = null;
            e.printStackTrace();
        }

        setTitle(ServiceAgentUtils.getAgentName(userJID));
        log.addInfo("Initializing ConfigStreamAgent : " + this.title + " Device NID ->" + deviceNID);
        resourceNIDList.clear();

        Document assetResourceDoc = MRXMLUtils.getDocumentXpath(setServiceDomainResponseDoc, "//" + AMConst.AssetResource);

        if (assetResourceDoc != null) {
            String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), "NID", null);

            if (assetResourceNID != null) {
                resourceNIDList.add(assetResourceNID);
            }
        }

        this.deviceType = ServiceAgentUtils.getDeviceType(assetResourceDoc);

        Element portResourcEList = MRXMLUtils.getElementXpath(setServiceDomainResponseDoc, "//PortResourceNIDList");
        List<?> listOfPorts = MRXMLUtils.getListXpath(portResourcEList, "resourceNID");

        for (Object object : listOfPorts) {
            resourceNIDList.add(MRXMLUtils.getValue((Element) object));
        }
    }

    protected void sendPortResourceTypeRequestToAppServer(String localConfigFile, String deviceNID, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {        

    	String requestName = MRRequest.getRequestName(SetPortResourceTypeRequestHandler.class.getName());          
    	
    	try {
    		
    		Document configDoc = MRXMLUtils.loadXMLFile(localConfigFile, log);
    		if(deviceNID == null || deviceNID.isEmpty()) {
    			return;
    		}
    		List<?> portConfigElList =  MRXMLUtils.getListXpath(configDoc, "//XP200PortConfig");
    		if(portConfigElList == null || portConfigElList.isEmpty()) {
    			return;
    		}

    		String assetResourceNID =  resourceNIDList.get(0);
    		Document assetRequestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log,AMConst.PORT.toLowerCase() ,  requestName + ".xml");
    		String elementName = MRRequest.getRequestRootName(requestName);
    		Element requestElement = MRXMLUtils.getElementXpath(assetRequestDoc, "//"+elementName);
    		MRXMLUtils.setAttributeValue(requestElement, AMConst.BY_ASSET_NID_ATTRIBUTE, assetResourceNID);

    		Element portResourceEl = MRXMLUtils.getFirstChild(requestElement);
    		portResourceEl.detach();
    		for(Object object:portConfigElList) {
    			Element portConfigEl  = (Element) object;
    			String portId  = MRXMLUtils.getValueXpath(portConfigEl, "./PortID","");
    			MRXMLUtils.setAttributeValue(portResourceEl, AMConst.DEVICE_ELEMENT_ID_ATTRIBUTE, portId);
    			String portType  = MRXMLUtils.getValueXpath(portConfigEl, "./Info/PortIOType","");
    			MRXMLUtils.setAttributeValue(portResourceEl, AMConst.TYPE_ATTRIBUTE, portType);
    			MRXMLUtils.addElement(requestElement,(Element)portResourceEl.clone());
    		}        		
    		String requestToken = DeviceConst.INIT_PORT_TYPE_UPDATE_TASK_NAME;    		
    		
    		MRXMLUtils.setValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, requestToken);
    		String fromAgentClassName = ConfigStreamAgent.class.getName();
    		String xmppAgentJID = AppServerAgentUtils.getAppServerAgentJID(this.userJID,serviceAgentFactory);
    		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, this.userJID, assetRequestDoc, log, requestName, fromAgentClassName, xmppAgentJID);
    		AgentInitUtils.updateAgentStateTask(assetRequestDoc, DeviceConst.INIT_PORT_TYPE_UPDATE_TASK_NAME,AgentInitUtils.AGENT_STATE_WAITING_FOR_RESPONSE_FROM_APPSERVER);

    	} catch (Exception e) {
    		e.printStackTrace();
    		log.addInfo("Error While sending request to appserver. requestName:="+requestName ,MRLog.INFO, MRLog.NOTIFY);
    	}
    }
    
    protected void initializeDeviceConfigInDB(String localConfigFile, String deviceNID,  XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {
    	
    	Document defaultDocument=null;
    	Element configEl=null;
        
        
        log.addInfo("ConfigStreamAgent : initializeDeviceStatusInDB Start", MRLog.INFO, MRLog.NOTIFY);
        try {
			defaultDocument = MRXMLUtils.loadXMLFile(localConfigFile, log);
	        configEl = MRXMLUtils.getElementXpath(defaultDocument, "//DeviceConfig");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
		
		sendUpdateConfigRequestToAppserver (configEl, null, "//DeviceConfig", deviceNID, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
        log.addInfo("ConfigStreamAgent : initializeDeviceStatusInDB End", MRLog.INFO, MRLog.NOTIFY);
    }
    
    protected void initializeDeviceStatusInDB(String localConfigFile, String deviceNID,  XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {
    	
    	Document defaultDocument=null;
    	Element statusEl=null;
        
        
        log.addInfo("ConfigStreamAgent : initializeDeviceStatusInDB Start", MRLog.INFO, MRLog.NOTIFY);
        try {
			defaultDocument = MRXMLUtils.loadXMLFile(localConfigFile, log);
	        statusEl = MRXMLUtils.getElementXpath(defaultDocument, "//DeviceStatus");
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		sendUpdateStatusRequestToAppserver (statusEl, "//DeviceStatus", deviceNID, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
        log.addInfo("ConfigStreamAgent : initializeDeviceStatusInDB End", MRLog.INFO, MRLog.NOTIFY);
    }
    
    //send status update request to appserver
    protected void sendUpdateStatusRequestToAppserver(Element updateElement, String updateXPath, String deviceNID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {
    
    	String data = "";
		try {
			data = MRXMLUtils.elementToString(updateElement);
		}catch(Exception exp) {
		    return;
		}
		String requestName = MRRequest.getRequestName(SetServiceClientStatusRequestHandler.class.getName());	
		
    	this.sendRequestToAppserver(data, updateXPath, MRConst.UPDATE, deviceNID, requestName, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
    	
    }
    
    //send config updates  to appserver    
    protected void sendUpdateConfigRequestToAppserver(Element updateElement, String updateParentXPath, String updateXPath, String deviceNID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {
    	String operation = MRConst.DELETE;
    	if (updateElement != null) {
			operation = MRConst.ADD;
			if (updateParentXPath == null) {
				operation = MRConst.UPDATE;
			}
		}
		
    	String xpath = updateXPath;    	
    	if(updateParentXPath != null) {
			xpath = updateParentXPath;
		} 
		String requestName = MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName());
		
		String data = "";
		try {
			data = MRXMLUtils.elementToString(updateElement);
		}catch(Exception exp) {
			//ignore
		}
		
		sendRequestToAppserver(data, xpath, operation, deviceNID, requestName, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
    }
    
    //send config and status update to appserver
    protected void sendRequestToAppserver(String data, String xpath,String operation,String deviceNID, String requestName,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) {
      	
    	if ((deviceNID == null) || deviceNID.isEmpty()) {
    		return;
    	}
    	
    	String requestToken = "";
    	try {   
    		
    		//using xpath as task Name
    		requestToken = AgentInitUtils.getRequestTokenForGivenTaskName(this.agentInitTaskDoc,xpath);
            	
    		Document assetRequestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log,AMConst.Device.toLowerCase() ,  requestName + ".xml");
    		String elementName = MRRequest.getRequestRootName(requestName);
    		Element requestElement = MRXMLUtils.getElementXpath(assetRequestDoc, "//"+elementName);
    		MRXMLUtils.setAttributeValue(requestElement, MRConst.XPATH_ATTRIBUTE, xpath);
    		MRXMLUtils.setAttributeValue(requestElement, MRConst.OPERATION_ATTRIBUTE, operation);
    		MRXMLUtils.setAttributeValue(requestElement, SessionServiceResourceConst.BY_NID_ATTRIBUTE, deviceNID);
    		
    		try {
    			Element updateElement  = MRXMLUtils.stringToElement(data);
    			MRXMLUtils.addElement(requestElement,(Element)updateElement.clone());
    		} catch (Exception e) {				
    			MRXMLUtils.setValue(requestElement, data);
    		}
    		
    		MRXMLUtils.setValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, requestToken);
    	
    		String fromAgentClassName = ConfigStreamAgent.class.getName();
    		String xmppAgentJID =  AppServerAgentUtils.getAppServerAgentJID(this.userJID,serviceAgentFactory);
    		log.addInfo("ConfigStreamAgent: "+ requestName +" operation:="+operation +" updateXpath:="+xpath ,MRLog.INFO, MRLog.NOTIFY);
    		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, this.userJID, assetRequestDoc, log, requestName, fromAgentClassName, xmppAgentJID);
    		if(this.agentInitTaskDoc !=null ) {
    			AgentInitUtils.updateAgentStateTask(this.agentInitTaskDoc, xpath, AgentInitUtils.AGENT_STATE_WAITING_FOR_RESPONSE_FROM_APPSERVER);    					
    		}
    		
    	} catch (Exception e) {
    		e.printStackTrace();
    		log.addInfo("Error While sending request to appserver. requestName:="+requestName +" operation:="+operation +"updateXpath:="+xpath ,MRLog.INFO, MRLog.NOTIFY);
    		if(this.agentInitTaskDoc !=null ) {
    			try {
					AgentInitUtils.updateAgentStateTask(this.agentInitTaskDoc, xpath, AgentInitUtils.AGENT_STATE_ERROR);
				} catch (Exception e1) {
				}    					
    		}
    	}
    }


    /**
     * @return the deviceNID
     */
    public String getDeviceNID() {
        return deviceNID;
    }

    /**
     * @param deviceNID the deviceNID to set
     */
    public void setDeviceNID(String deviceNID) {
        this.deviceNID = deviceNID;
    }

    public void handleConfigUpdates(String localConfigFile, String updateParentXPath, String updateXPath, String userJID, ServiceMessageHandlerI messageHandler)
        throws MRException, Exception {
    	Element el=null;
    	Document docConfig = MRXMLUtils.loadXMLFile(localConfigFile, log);
    	try {
    		el = MRXMLUtils.getElementXpath(docConfig, updateXPath);
    	}
    	catch (Exception e) {
    	}
    	
		sendUpdateConfigRequestToAppserver (el, updateParentXPath, updateXPath, deviceNID, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID);
    	
//        if ((updateXPath == null) || (updateXPath.contains("XP200PortConfig/Info"))) {
//            sendPortResourceTypeRequestToAppServer(localConfigFile, deviceNID, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
//        }
    }
    public void handleStatusUpdates(String updateXPath,  String userJID, ServiceMessageHandlerI messageHandler)
    throws MRException, Exception {
    	Document docStatus = MRXMLUtils.loadXMLFile(DeviceConst.DEVICESTATUSFILE, log);
    	Element el = MRXMLUtils.getElementXpath(docStatus, updateXPath);
		sendUpdateStatusRequestToAppserver (el, updateXPath, deviceNID, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID);
//		validateStatusUpdates(log, db, userJID, deviceNID, updateXPath);
    }
        
    public void handleErrorStatusUpdates(Element element, String state, String userJID, ServiceMessageHandlerI messageHandler)
    throws MRException, Exception {
    	String childXpath = "//" + element.getName() + "/@" + AMConst.STATE;
     	String data= " attribute "+AMConst.STATE+" {\"" + state + "\"}  ";
    	String requestName = MRRequest.getRequestName(SetServiceClientStatusRequestHandler.class.getName()); 
    	this.sendRequestToAppserver(data, childXpath, MRConst.UPDATE_ATTRIBUTE, deviceNID, requestName,xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID);
    }

    public void sendUpdateStatusAndGetInvitesRequestsToAppServer() throws MRException, Exception {
        String requestName = MRRequest.getRequestName(GetMediaRoomInvitesRequestHandler.class.getName());
        Document mediaRoomRequest = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, requestName);
        MRXMLUtils.setValueXpath(mediaRoomRequest, "//contactJID", this.userJID);

        Element resourceNIDListElement = MRXMLUtils.getElementXpath(mediaRoomRequest, "//resourceNIDList");

        for (String resourceNID : this.resourceNIDList) {
            MRXMLUtils.addElement(resourceNIDListElement, MRXMLUtils.stringToElement("<resourceNID>" + resourceNID + "</resourceNID>"));
        }

        fillTheDirInfoInGetMediaRoomInvitesRequest(mediaRoomRequest); 
        fillTheMediaInfoInGetMediaRoomInvitesRequest(mediaRoomRequest);

        MediaRoomServiceUtils.sendMessageToAppServerAgent(xf.xmpp(userJID), userJID, mediaRoomRequest, log, requestName, ConfigStreamAgent.class.getName(),  AppServerAgentUtils.getAppServerAgentJID(this.userJID,xf.sf(userJID)));
    }
    
    private void fillTheDirInfoInGetMediaRoomInvitesRequest(Document mediaRoomRequest) throws MRException,Exception {
    	 MediaStoreAdminServerAgent storeAgent = (MediaStoreAdminServerAgent) this.xf.sf(userJID).getServiceAgent(MediaStoreAdminServerAgent.class.getName());
	        List<String> dirNIDList = null;
	
	        if (storeAgent != null) {
	            dirNIDList = storeAgent.getDirNIDList();
	            Element mediaStreamdirNIDListElement = MRXMLUtils.getElementXpath(mediaRoomRequest, "//mediaStreamDirNIDList");
	
	            if (dirNIDList != null) {
	                for (String dirNID : dirNIDList) {
	                    MRXMLUtils.addElement(mediaStreamdirNIDListElement,MRXMLUtils.stringToElement("<dirNID>" + dirNID + "</dirNID>"));
	                }
	            }
	        }
    }

    private void fillTheMediaInfoInGetMediaRoomInvitesRequest(Document mediaRoomRequest) throws MRException,Exception {
   	 	DestStreamAgent destAgent = (DestStreamAgent) this.xf.sf(userJID).getServiceAgent(DestStreamAgent.class.getName());
   	 	Element mediaStreamdirNIDListElement = MRXMLUtils.getElementXpath(mediaRoomRequest, "//mediaStreamDirNIDList");
   	        Element parent = mediaStreamdirNIDListElement.getParent();
        if (destAgent != null) {
            
            Element streamInfoList = destAgent.getStreamInfoListElement();
            MRXMLUtils.addElement(parent, streamInfoList);
        }
        SourceStreamAgent sourceAgent = (SourceStreamAgent)this.xf.sf(userJID).getServiceAgent(SourceStreamAgent.class.getName());
        
        if(sourceAgent != null){
            Element sourceStreamInfoList = sourceAgent.getStreamInfoListElement();
            MRXMLUtils.addElement(parent, sourceStreamInfoList);
        }
   }

    
    public void sendGetInvitesRequestsToAppServerForAResource(String resourceNID,String resourceType) throws MRException, Exception {
        String requestName = MRRequest.getRequestName(GetMediaRoomInvitesRequestHandler.class.getName());
        Document mediaRoomRequest = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, requestName);
        MRXMLUtils.setValueXpath(mediaRoomRequest, "//contactJID", this.userJID);

        Element resourceNIDListElement = MRXMLUtils.getElementXpath(mediaRoomRequest, "//resourceNIDList");
        MRXMLUtils.addElement(resourceNIDListElement, MRXMLUtils.stringToElement("<resourceNID>" + resourceNID + "</resourceNID>"));
       

        if(resourceType.equals(AMConst.MEDIA_STORE_PORT_RESOURCE)) {
        	fillTheDirInfoInGetMediaRoomInvitesRequest(mediaRoomRequest);
        }

        MediaRoomServiceUtils.sendMessageToAppServerAgent(xf.xmpp(userJID), userJID, mediaRoomRequest, log, requestName, ConfigStreamAgent.class.getName(),  AppServerAgentUtils.getAppServerAgentJID(this.userJID,xf.sf(userJID)));
    }

	public boolean isDBTransferEvent(Document messageDoc, MRLog log) throws MRException,Exception{
		String serviceName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.FTP_SERVICE);
		if(serviceName.equals(MRConst.FTP_SERVICE)){
            if(HAUtils.isHAServiceEnabled()){
            	return true;
            }
        }
		return false;
	}
	
	
//	protected void updateResourceStateAsError() throws MRException,Exception {
//		for (String resourceNID : this.resourceNIDList) {
//			AssetAdminServiceUtils.updateState(resourceNID, MRClientUtils.ASSET_STATE_ERROR, "", this.userJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
//        }
//	}

	/**
	 * @return the localConfigFile
	 */
	public String getLocalConfigFile() {
		return localConfigFile;
	}
		
	protected void sendAgentStateEventToMRClientAgent() throws MRException,Exception {		
		log.addInfo("ConfigStreamAgent : Agent Intialized send AgentStateEvent ", MRLog.INFO, MRLog.NOTIFY);
		ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), ConfigStreamAgent.class.getName(), MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);	
	}

	
	public boolean isAgentInitFailed() {
		if(this.agentInitTaskDoc != null) {
			Element taskEl = null;
			try {
				taskEl = MRXMLUtils.getElementXpath(this.agentInitTaskDoc, "//Task[@state='"+AgentInitUtils.AGENT_STATE_ERROR+"']");
				if(taskEl != null) {
					return true;
				}
			} catch (Exception e) {
			}
		}
		
		return false;
	}

	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub
		
	}
  }
