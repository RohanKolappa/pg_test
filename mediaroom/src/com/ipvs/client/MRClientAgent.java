package com.ipvs.client;

import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.packet.IQ.Type;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.assetadminservice.device.handler.SetServiceClientStatusRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.GetMediaStreamFileMaxRevisionRequestHandler;
import com.ipvs.assetadminservice.port.handler.SetPortResourceTypeRequestHandler;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventHandlerThread;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.DeviceAdminClientAgent;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.policyadminservice.impl.PolicyAdminAgent;
import com.ipvs.sessionservice.resource.handler.UpgradeDeviceRequestHandler;
import com.ipvs.systemservice.handler.DownloadDBRequestHandler;
import com.ipvs.systemservice.handler.SetServiceDomainRequestHandler;
import com.ipvs.systemservice.impl.FileTransferAgent;
import com.ipvs.xmpp.client.DBUpdateEvent;
import com.ipvs.xmpp.client.smack.DiscoveryServiceI;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;
import com.ipvs.xmpp.client.smack.packets.ResponseElement;
import com.ipvs.xmpp.client.smack.packets.SessionIQ;


public class MRClientAgent implements MRResponseHandlerI,MRClient {
    private static boolean useAppServer = true;
    static boolean LEGACYRESPONSE = false;
    int loglevel;

    DeviceAdminClientAgent deviceAdminClientAgent;
    FileTransferAgent fileTransferAgent;
    PolicyAdminAgent policyAdminAgent;
    AppServerAgent appServerAgent;
    XMPPEventHandlerThread xmppEventHandlerThread;
    ArrayList<XMPPEvent> cachedEvents;
    ArrayList<String> cachedLogs;
    private ServiceMessageHandlerI messageHandler;
    private boolean isInitialized = false;
    private boolean isReady = false;
    int runningNumber = 0;
    Document assetDoc = null;
    private String userJID;
    private XMPPI xmpp;
    private MRLog log;
    private MRDB db;
    private MRResourceFactoryI resourceFactory;
    private ServiceAgentFactoryI serviceAgentFactory;
    private String resourceGroup; 
    DiscoveryServiceI discoveryService = null;
    public static HashMap<String,String> serviceToAgentMapping = null; 
    XMPPF xf;
    		
    
    
    @Override
    public XMPPI getXmpp() {
        return xmpp;
    }

    @Override
    public MRLog getLog() {
        return log;
    }

    @Override
    public MRDB getDb() {
        return db;
    }

    @Override
    public MRResourceFactoryI getResourceFactory() {
        return resourceFactory;
    }

    @Override
    public ServiceAgentFactoryI getServiceAgentFactory() {
        return serviceAgentFactory;
    }

   
    @Override
    public String getResourceGroup() {
        return resourceGroup;
    }

    @Override
    public String getUserJID() {
        return this.userJID;
    }  
    
	
	public DiscoveryServiceI getDiscoveryService() {	
		return discoveryService;
	}
	
	public void setDiscoveryService(DiscoveryServiceI discoveryService) {
		this.discoveryService = discoveryService;
	}

    @Override
    public void reset() {     
    	Iterator<String> agentIteration = this.serviceAgentFactory.getIterator();
    	while(agentIteration.hasNext()) {
    		ServiceAgentI serviceAgent = serviceAgentFactory.getServiceAgent(agentIteration.next());
    		try {
				serviceAgent.reset();
			} catch (Exception e) {
				e.printStackTrace();
			}
    	}
        this.db = null;
        this.resourceFactory = null;
        this.serviceAgentFactory = null;
        this.resourceGroup = null;
        this.userJID = null;
        this.messageHandler = null;
        this.deviceAdminClientAgent = null;
        this.policyAdminAgent = null;
        this.appServerAgent = null;
        this.cachedEvents = null;
        //this.xmpp = null;
    }

    public MRClientAgent(XMPPF xf, XMPPI xmpp,String userJID, String resourceID, String serverName, ServiceMessageHandlerI messageHandler){
    	
    	this.xf = xf;
	    this.xmpp = xmpp;
        this.userJID = userJID;
        this.resourceGroup = resourceID;
        this.log = MRLog.getInstance(MRLog.CONSOLE);
        this.db = new MRDB(xf, userJID, log);
        this.serviceAgentFactory = new ServiceAgentFactory();
    	this.messageHandler = messageHandler;
    	this.deviceAdminClientAgent = new DeviceAdminClientAgent();
    	this.fileTransferAgent = new FileTransferAgent();
    	this.policyAdminAgent = PolicyAdminAgent.getPolicyAdminAgent();
    	this.appServerAgent = new AppServerAgent();    
    	this.cachedEvents = new ArrayList<XMPPEvent>();
    	this.addServiceAgent(userJID, this.policyAdminAgent);
    	this.addServiceAgent(userJID, this.deviceAdminClientAgent);
    	this.addServiceAgent(userJID, this.fileTransferAgent);
    	this.addServiceAgent(userJID, this.appServerAgent);  
    	
    	serviceToAgentMapping   = new HashMap<String, String>();
    	//new service names
    	serviceToAgentMapping.put(MRConst.MEDIA_SERVICE, SourceStreamAgent.class.getName());
    	serviceToAgentMapping.put(MRConst.LAYOUT_SERVICE, DestStreamAgent.class.getName());
    	serviceToAgentMapping.put(MRConst.DEVICE_ADMIN_SERVICE, DeviceAdminServerAgent.class.getName());    	
    	serviceToAgentMapping.put(MRConst.MEDIA_STORE_ADMIN_SERVICE, MediaStoreAdminServerAgent.class.getName());
    	serviceToAgentMapping.put(MRConst.PUBSUB_SERVICE, AppServerAgent.class.getName());    	
    	
    	//TBR
    	serviceToAgentMapping.put(MRConst.CONFIG_ADMIN_SERVICE, ConfigStreamAgent.class.getName());
    	
    	//OLD service names
    	serviceToAgentMapping.put(ServiceClient.ASSETADMINSERVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.ASSETADMINSERVICE_DEVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.ASSETADMINSERVICE_FILE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.ASSETADMINSERVICE_DIR, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.ASSETADMINSERVICE_PORT, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.POLICYADMINSERVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SYSTEMSERVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SESSIONSERVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SESSIONSERVICE_RESOURCE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SESSIONSERVICE_SERVICE, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SESSIONSERVICE_SESSION, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.SESSIONSERVICE_WINDOW, AppServerAgent.class.getName());
    	serviceToAgentMapping.put(ServiceClient.MEDIAROOMSERVICE, AppServerAgent.class.getName());
    	
    	
    	
    	serviceToAgentMapping.put(MRConst.FTP_SERVICE, FileTransferAgent.class.getName());
    
    }

    public static void setLegacyResponse(boolean legacyResponse) {
    	MRClientAgent.LEGACYRESPONSE = legacyResponse;
    }

    public static boolean isAppServerEnabled() {
        return useAppServer;
    }

    public void init(Document setServiceDomainRequestDoc, boolean resetDB, MRResponseHandlerI responseHandler, int logLevel)
        throws MRException, Exception {
        Element domainElement = MRXMLUtils.getElementXpath(setServiceDomainRequestDoc, "//domain");
        String serviceDomain = MRXMLUtils.getValue(domainElement);
        String rootNID = "MEDIAROOM.SERVICE.DOMAIN." + serviceDomain;        
        MRXMLDBUtils.initDB(db, log, rootNID, serviceDomain, MRConst.MRSCHEMAPATH, resetDB);
        this.xmpp.setSuperUser(MRServer.ADMIN);
        this.log.setLogLevel(logLevel);     
   
        appServerAgent = (AppServerAgent) this.serviceAgentFactory.getServiceAgent(MediaRoomServiceConst.APPSERVERAGENT);

      
        appServerAgent.init(this.xmpp, this.log, this.db, resourceFactory, this.serviceAgentFactory, this.userJID, null);

        //Initialize Policy Admin Agent Only in App server
        if (!ServiceAgentUtils.matchesJID(this.userJID,  AppServerAgentUtils.getAppServerAgentJID(this.userJID,serviceAgentFactory))) {
            this.serviceAgentFactory.removeAgent(PolicyAdminAgent.class.getName());
        }
      
    }

    private synchronized void initializeAgents(Document setServiceDomainResponseDoc) throws Exception {
        if (this.isInitialized) {
            return;
        }

        Document assetResourcedoc = MRXMLUtils.getDocumentXpath(setServiceDomainResponseDoc, "//" + AMConst.AssetResource);
        this.assetDoc = assetResourcedoc;       
        Iterator<String> agents = this.serviceAgentFactory.getIterator();

        while (agents.hasNext()) {
            String name = agents.next();

            if (name.equalsIgnoreCase(MediaRoomServiceConst.APPSERVERAGENT)) {
                continue;
            }

            ServiceAgentI agent = this.serviceAgentFactory.getServiceAgent(name);
            agent.init(this.xf, this.log, this.db, resourceFactory, this.userJID, setServiceDomainResponseDoc);
        }
        
        Iterator<XMPPEvent> events = this.cachedEvents.iterator();
        int i = 0;

        System.out.println("Handling cached events Begin");

        while (events.hasNext()) {
            i++;
            this.log.addInfo("MRClientAgent: handling cached event#=" + i);

            try {
                String notificationXML = this.handleXMPPEventInternal(events.next());
                if (notificationXML != null) {
                    messageHandler.handleServiceMessage(userJID, notificationXML);
                }
            } catch (Exception e) {
                this.log.addInfo("MRClientAgent:initInternal exception handling cached event#=" + i + e.toString());
            }
        }

        System.out.println("Handling cached events End");

        this.cachedEvents.clear();
        this.isInitialized = true;

        verifyAllAgentsInReadyState();
    }
    
    public void verifyAllAgentsInReadyState() { 
    	
    	log.addInfo("MRClientAgent: Verify all the agents are in ready state.", MRLog.INFO, MRLog.NOTIFY);    	
    	for (Iterator<String> agentList = this.serviceAgentFactory.getIterator(); agentList.hasNext();) {
    		if (!this.serviceAgentFactory.getAgentState(agentList.next())) {
    			return;
    		}
    	}       	
    	this.setReady(true);    	
    }

    public synchronized boolean isReady() {
        return isReady;
    }

    protected synchronized void setReady(boolean isReady) {
    	
    	//if the client agent is ready just ignore the agent state event
    	if(this.isReady == true) {
    		return;
    	}
    	
        this.isReady = isReady;

        if (isReady == true) {
            log.addInfo("MRClientAgent: Set State As Ready, Ready to Accept Room Events.", MRLog.INFO, MRLog.NOTIFY);

            //send UpdateResourceStatus request to App Server Agent             
            try {
            	if(AppServerAgent.isXMPPEnabled(this.userJID)) {
            	    appServerAgent.handleReady();
            	}
                ConfigStreamAgent cfgAgent = (ConfigStreamAgent)  this.serviceAgentFactory.getServiceAgent(ConfigStreamAgent.class.getName());
                cfgAgent.sendUpdateStatusAndGetInvitesRequestsToAppServer();
            } catch (Exception exp) {
                exp.printStackTrace();
            }
        }
    }

    public synchronized boolean getInitialized() throws Exception {
        return this.isInitialized;
    }

    public boolean isReadyForEvents(String message, XMPPEvent event) throws Exception {
    	String messageRootElement = MRXMLUtils.stringToDocument(message).getRootElement().getName();
        if (!this.isReady()) {
        	
        	//if the user received through link local then process the request
        	if(MRClientUtils.isLinkLocalJID(event.getFrom())) {
        		this.log.addInfo("Link Local Message From UserJID:="+event.getFrom());
        		return true;
        	}
        	
            //If not initialized then cache the event. If initialized then handle the Request/Response
            if (message.indexOf(MRRequest.getRequestName(GetMediaStreamFileMaxRevisionRequestHandler.class.getName())) != -1) {
                message = URLDecoder.decode(message, "UTF-8");

                Document messageDoc = MRXMLUtils.stringToDocument(message);
                String messagetype = MRXMLUtils.getAttributeValueXpath(messageDoc, "//AgentMessage","type", "");

                if (ServiceClient.REQUEST.equals(messagetype) && !this.isInitialized) {                    
                    this.cachedEvents.add(event);
                    this.log.addInfo("MRClientAgent: caching event size=" + this.cachedEvents.size());
                    return false;
                }

                return true;
            } else if(messageRootElement.trim().equals(ServiceAgentUtils.PRESENCE)) {
            	this.cachedEvents.add(event);
                return false;
            	
            } else if (message.indexOf(MRConst.AGENT_STATE) != -1) {
                // If the message is  AgentState AgentEvent then handle the message
                return true;
            } else if (message.indexOf(MRRequest.getRequestName(DownloadDBRequestHandler.class.getName())) != -1) {
            	//if the message is download db response then handle the response
                return true;
            }else if (message.indexOf(MRRequest.getRequestName(SetServiceDomainRequestHandler.class.getName())) != -1) {
            	//if the message is set service domain request then handle the response
                return true;
            } else if(AppServerAgentUtils.isServiceResourceUpdateEvent(message,log,db,userJID,getServiceAgentFactory())) {
            	//if appserver ServiceResource update then handle that update
            	return true;
            } else if(ServiceAgentUtils.isShutdownEvent(message)){
            	//if the event is shutdown event forward that event to app/applet
            	return true;
            } else if (message.indexOf(MRRequest.getRequestName(UpgradeDeviceRequestHandler.class.getName())) != -1) {
            	//if the device is in NeedsUpgrade state then initiate UpgradeDeviceRequestHandler
                return true;
            } else if (message.indexOf(MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName())) != -1 ||
            		 message.indexOf(MRRequest.getRequestName(SetServiceClientStatusRequestHandler.class.getName())) != -1  || 
            		 message.indexOf(MRRequest.getRequestName(SetPortResourceTypeRequestHandler.class.getName())) != -1 ) {
            	//if the responses for device config and status updates handle the responses to intialize the config agent
                return true;
            }else {
            	Document messageDoc = MRXMLUtils.stringToDocument(message);
            	if(isInitialized) {            		
	            	// if the event is media stream dir update event handle the dir update
	            	MediaStoreAdminServerAgent storeAgent = (MediaStoreAdminServerAgent) this.getServiceAgentFactory().getServiceAgent(MediaStoreAdminServerAgent.class.getName());
	            	if(storeAgent != null) {
	            		if(storeAgent.isMediaStreamDirEvent(messageDoc,db,log) != null) {
	            			return true;
	            		}
	            	}
	            	//if agents are initialized  and the HA is enabled then allow file transfer agents
	            	ConfigStreamAgent cfgAgent = (ConfigStreamAgent) this.getServiceAgentFactory().getServiceAgent(ConfigStreamAgent.class.getName());
	            	if(cfgAgent!= null){
	            		if(cfgAgent.isDBTransferEvent(messageDoc,log)){
	            			return true;
	            		}
	            	}
            	}
            	
            	// Return true if IQ event
                String eventType = messageDoc.getRootElement().getName();
                if(ServiceClient.IQ.equals(eventType)) {
                	return true;
            	}
            }
        }

        return this.isReady();
    }
    
    
    private void handleAgentResourceStateEvent(String message, XMPPEvent event) throws MRException, Exception {
    	
    	//ignore AgentResourceEvent event if the agent itself is not ready
    	if(!this.isReady){
    		return;
    	}
    	
    	Document messageDoc= MRXMLUtils.stringToDocument(message);
    	String agentName = MRXMLUtils.getValueXpath(messageDoc, "//AgentName", "");
    	String agentResourceState = MRXMLUtils.getValueXpath(messageDoc, "//AgentResourceState", "");
     	
    	if(agentResourceState.equals(MRClientUtils.ASSET_STATE_READY)) {
    		String agentResourceNID = MRXMLUtils.getValueXpath(messageDoc, "//AgentResourceNID", "");
        	String agentResourceType = MRXMLUtils.getValueXpath(messageDoc, "//AgentResourceType", "");
       
    		log.addInfo("MRClientAgent:handleAgentResourceStateEvent AgentName:="+agentName + " AgentResourceNID:="+agentResourceNID +" AgentResourceType:="+agentResourceType
    				+" AgentResourceState:="+agentResourceState, MRLog.INFO, MRLog.NOTIFY);
    		ConfigStreamAgent cfgAgent = (ConfigStreamAgent)  this.serviceAgentFactory.getServiceAgent(ConfigStreamAgent.class.getName());
            cfgAgent.sendGetInvitesRequestsToAppServerForAResource(agentResourceNID, agentResourceType);
    	}
    	
    	
    }

    private void handleAgentStateEvent(String message, XMPPEvent event) throws MRException, Exception {
    	
    	//ignore agent state events if the mrclient agent is ready
    	if(this.isReady){
    		return;
    	}
    	
    	Document messageDoc= MRXMLUtils.stringToDocument(message);
    	String agentName = MRXMLUtils.getValueXpath(messageDoc, "//AgentName", "");
    	String agentState = MRXMLUtils.getValueXpath(messageDoc, "//AgentState", "");
    	
    	if(agentState.equals(MRClientUtils.ASSET_STATE_READY)) {
    		this.getServiceAgentFactory().setAgentStateAsReady(agentName);
    		log.addInfo("MRClientAgent: Set AgenState As Ready. AgentName:="+agentName, MRLog.INFO, MRLog.NOTIFY); 
    	}
    	
    	verifyAllAgentsInReadyState();
    }

    public void addServiceAgent(String userJID, ServiceAgentI serviceAgent) {
        this.addServiceAgent(userJID, serviceAgent.getClass().getName(), serviceAgent);
    }

    public void addServiceAgent(String userJID, String agentName, ServiceAgentI serviceAgent) {
        this.serviceAgentFactory.addAgent(ServiceAgentUtils.getAgentName(agentName, userJID), serviceAgent);
    }

    public void removeServiceAgent(String userJID, String name) {
        this.serviceAgentFactory.removeAgent(ServiceAgentUtils.getAgentName(name, userJID));
    }

    public String handleSetServiceDomain(Document requestDoc, boolean resetDB)
        throws Exception {
        this.init(requestDoc, resetDB, this, this.loglevel);        
        return this.sendRequest(MRXMLUtils.documentToString(requestDoc));
        
    }

    private void handleSetServiceDomainResponse(Document responseDoc) throws MRException,Exception {
    	String message = null;
        try {
        	log.addInfo("MRClientAgent: Handle SetServiceDomainResponse.",MRLog.INFO,MRLog.NOTIFY);
        	message = MRXMLUtils.documentToString(responseDoc);
        	
        	if(!ServiceAgentUtils.isSetServiceDomainFailed(message,this.log)) {
                       this.initializeAgents(responseDoc);
        	}
        	
        } catch (Exception exp) {
        	MRXMLUtils.setAttributeValueXPath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, String.valueOf(MRRequest.STATE_EXCEPTION));        	
            exp.printStackTrace();
        }        
    }

    /*
    <iq from='roomuser-default@localhost/userpc' 
  	     id='zzz-123' 
  	     to='managementserver@localhost/managementserver' 
  	     type='set/get'> 
  	     <query xmlns='com.ipvs.session'> 
  	     	<!-- Request data goes here --> 
  	     </query> 
  	   </iq>

    */

    public void sendIQ(Document doc) throws Exception {
    	// System.out.println("MRClientAgent:sendIQ=" + MRXMLUtils.documentToString(doc));
    	Element iq = MRXMLUtils.getElementXpath(doc, "//iq");
    	String from = MRXMLUtils.getAttributeValue(iq,"from");
    	String id = MRXMLUtils.getAttributeValue(iq,"id");
    	String to = MRXMLUtils.getAttributeValue(iq,"to");
    	//String type = MRXMLUtils.getAttributeValue(iq,"type");
    	
    	Element queryElement = ((Element)doc.getRootElement().elements().iterator().next());
    	String queryElementXML = MRXMLUtils.elementToString(queryElement);
    	// System.out.println("MRClientAgent:sendIQ dataElementXML=" + dataElementXML);
    	
    	SessionIQ sessionElement = new SessionIQ();
    	sessionElement.setFrom(from);
    	sessionElement.setPacketID(id);
    	sessionElement.setTo(to);
    	sessionElement.setType(Type.SET);
    	sessionElement.setChildElementXML(queryElementXML);
    	// System.out.println("MRClientAgent:sendIQ packet=" + sessionElement.toXML());
    	this.xmpp.sendPacket(sessionElement);
    }

    public String sendRequest(String messageXML) throws Exception {
        // _log.addTraceBegin(request); TBD
        if (userJID == null) {
            throw new MRException(MRExceptionCodes.LOGINNOTFOUND, "<error>" + "Login not found for sendRequest" + "</error>");
        }      

        // AgentMessage directed to our own userJID
        Document doc = MRXMLUtils.stringToDocument(messageXML);
        Element agentMessageElement = doc.getRootElement();
        String agentJID = MRXMLUtils.getAttributeValue(agentMessageElement, "to", "");
        
        //legacy support 
        if(agentJID.isEmpty()){
        	agentJID = MRXMLUtils.getValueXpath(agentMessageElement, ".//agentJID", "");
        }
        String requestNID = "none";

        if(ServiceClient.IQ.equals(doc.getRootElement().getName())) {
        	this.sendIQ(doc);
        	requestNID = ServiceClient.IQ;
        } else if ("".equals(agentJID.trim())) {
            Document requestDoc = doc;

            // Check if the requestDoc is embedded
            Element requestElement = MRXMLUtils.getElementXpath(doc, "//Request");

            if (requestElement != null) {
                requestDoc = MRXMLUtils.getDocument();
                requestDoc.setRootElement((Element) requestElement.detach());
            }
            
            //convert legacy requests to latest
            String request = MRClientAgentUtils.convertLegacyRequestsToLatetFormat(requestDoc.asXML());
            requestDoc = MRXMLUtils.stringToDocument(request);
            
            
            String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"");
       
            String roomNID = null;
            String agentClassName = AppServerAgent.class.getName();
            String fromAgentClassName = MRClientAgent.class.getName();
            String info = "MRClientAgent:=" + this.userJID + " > Send " + requestName + " to AppSeverAgent:=";
            ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, roomNID,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory), agentClassName, requestDoc, info, requestNID);
            runningNumber++;
            requestNID = String.valueOf(runningNumber);           

        } else {
        	
        	messageXML = MRClientAgentUtils.convertLegacyRequestsToLatetFormat(messageXML); 
        	
        	//TBR: remove this after ui check in his code to trunk
        	//messageXML = MRClientAgentUtils.convertLegacyAgentmessageToLatetFormat(messageXML, userJID);
        	
        	
        	String message = "<x xmlns='" + IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS + "'>" +
    		messageXML + 
    		"</x>";    		
        	//if the agent jid is link local agent jid send message through link local
        	 		// AgentMessage to be sent to external Agents  
        	this.xmpp.sendMessage(agentJID, message,
        				IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
        	requestNID = agentJID;
	    
        }
        return "<Request><requestNID>" + requestNID + "</requestNID></Request>";

        // this.log.addTraceEnd(request); TBD
    }

    public static Element getPresenceStatus(Element event) throws Exception {
        Iterator<?> i = event.elements().iterator();

        while (i.hasNext()) {
            Element x = (Element) i.next();

            if ("x".equals(x.getName())) {
                Iterator<?> j = x.elements().iterator();

                while (j.hasNext()) {
                    Element status = (Element) j.next();

                    if ("status".equals(status.getName())) {
                        return status;
                    }
                }
            }
        }

        return null;
    }

    private void handleInvite(String roomNID) {
        this.log.addInfo("MRClientAgent: userJID=" + this.getUserJID() + " received invite roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
        try {
            if (this.xmpp.isJoined(roomNID)) {
                this.log.addInfo("MRClientAgent: userJID=" + this.getUserJID() + " Warning not rejoining roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
            } else {
                this.log.addInfo("MRClientAgent: userJID=" + this.getUserJID() + " join roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
            	this.xmpp.joinRoom(roomNID, ServiceAgentUtils.getRoomNickname(this.getUserJID()));
            }
        } catch (Exception e) {
            this.log.addInfo("MRClientAgent: failed to join roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
            e.printStackTrace();
        }
    }    
    
	@SuppressWarnings("unused")
	private void handleRoomUnavailable(String roomNID) {
    	log.addInfo("MRClientAgent:=" + this.userJID + ":handleRoomUnAvailable leaving roomNID=" + roomNID,
    			MRLog.OK, MRLog.NOTIFY);
	    try {
	    	this.xmpp.leaveRoom(roomNID, userJID);
	    }
	    catch(Exception e) {
	        log.addInfo("MRClientAgent:=" + this.userJID + ":leaveRoom roomNID=" + roomNID
	        		+ " exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);	    	
	    	// e.printStackTrace();
	    }
    }

	/*
    public String handlePresence(String message, XMPPEvent event, Document doc, Element presence)
        throws Exception {
        String type = MRXMLUtils.getAttributeValue(presence,"type");
        String from = MRXMLUtils.getAttributeValue(presence,"from");
        String roomUserJID = "";
        try {
        	roomUserJID = MRXMLUtils.getAttributeValue(MRXMLUtils.getFirstChild(MRXMLUtils.getFirstChild(presence)),"jid");
        } catch(Exception e) {
	        log.addInfo("MRClientAgent:=" + this.userJID + ":handlePresence unabled to parse presence=" +
	        		message, MRLog.OK, MRLog.NOTIFY);
	        return message;
        }
        // if(!roomUserJID.equals(presenceUserJID)) {
        	// System.out.println("MRClientAgent:handlePresence Warning roomUserJID=" + roomUserJID + " != presenceUserJID=" + presenceUserJID);
        // }
        String roomNID = ServiceAgentUtils.getRoomNID(from);
        boolean available = !(ServiceAgentUtils.MUCUNAVAILABLE.equals(type));

        // Getting spurious not available messages
        if (!available) {
            Element status = MRClientAgent.getPresenceStatus(presence);

            try {
                int code = Integer.parseInt( MRXMLUtils.getAttributeValue(status,"code"));

                if (code == 303) { // Multiple joins on a room
                    this.log.addInfo("MRClientAgent: userJID=" + this.getUserJID() + " Ignoring unavailable due to multiple join event=" + message, MRLog.OK, MRLog.NOTIFY);

                    return message;
                }
            } catch (Exception e) {
                // Could not get status
            }
        }
        
        if (ServiceAgentUtils.roomUnvailable(presence, available)) {
            this.handleRoomUnavailable(roomNID);
        } else if(!available)
        	this.log.addInfo("MRClientAgent: roomUserJID=" + roomUserJID + " Got unavailable event=" + message, MRLog.OK, MRLog.NOTIFY);

        Iterator<String> agents = this.serviceAgentFactory.getIterator();

        while (agents.hasNext()) {
            String name = agents.next();
            this.serviceAgentFactory.getServiceAgent(name).handlePresence(this, doc, presence, roomUserJID, available, roomNID);
        }

        return message;
    }
    */

    String getUsedMemory(String tag) {
    	//Runtime.getRuntime().gc(); //Findbugs
    	long usedMemory = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
    	return "<" + tag + ">" + String.valueOf(usedMemory/1024) + "</" + tag + ">";
    }
    
    public void handleXMPPEvent(XMPPEvent event) throws Exception {
        String message = event.getMessage();
        if (!this.isReadyForEvents(message, event)) {
        	/*if(message!=null) {
            	String messageRootElement = MRXMLUtils.stringToDocument(message).getRootElement().getName();
            	if(messageRootElement.trim().equals(ServiceAgentUtils.PRESENCE)) {
            		String notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);
            		messageHandler.handleServiceMessage(userJID, notification);
            	}
        	}*/	
            return;
        }

        // this.log.addInfo(this.getUsedMemory("MB"), MRLog.OK, MRLog.NOTIFY);
        String notification = this.handleXMPPEventInternal(event);

        if (notification != null) {
            messageHandler.handleServiceMessage(userJID, notification);
        }
        
        // this.log.addInfo(this.getUsedMemory("ME"), MRLog.OK, MRLog.NOTIFY);
    }
    
    private void handleIQ(String fromJID, Document doc) throws Exception {
    	// System.out.println("MRClientAgent:handleIQ=" + MRXMLUtils.documentToString(doc));
		log.addInfo("MRClientAgent:handleIQ userJID=" + fromJID, MRLog.INFO,MRLog.NOTIFY);
        Element query = MRXMLUtils.getDescendentWithoutNamespace(doc.getRootElement(), "query");
        String xmlns = MRXMLUtils.getElementNameSpaceURI(query, "");
        String id = MRXMLUtils.getAttributeValue(doc.getRootElement(),"id");
        if(!IPVSMessageExtension.IPVS_SESSION_XMLNS.equals(xmlns)) {
            return;
        }
        String type = MRXMLUtils.getAttributeValue(doc.getRootElement(),"type");
        if("set".equals(type) || "get".equals(type)) {
            this.appServerAgent.handleSessionIQ(fromJID, query, id);
        }
    }

    private String handleXMPPEventInternal(XMPPEvent event) throws Exception {        
        String message = event.getMessage();
        String fromJID = event.getFrom();
        String notification = null;
        // message = URLDecoder.decode(message, "UTF-8");
        
        Document messageDoc = MRXMLUtils.stringToDocument(message);
     //   System.out.println("messageDoc:="+message);
        
        String eventType = messageDoc.getRootElement().getName();
        @SuppressWarnings("unused")
        String notificationType = null;  
        if(ServiceClient.IQ.equals(eventType)) {
        	handleIQ(fromJID, messageDoc);
        }
        else if (eventType.equals(ServiceAgentUtils.PRESENCE)) { //handle presence events
            Document doc = MRXMLUtils.getDocument(event.getMessage());
        	this.appServerAgent.handlePresence(this, doc, doc.getRootElement());
/*
            Element eventEL = MRXMLUtils.getDescendentWithoutNamespace(messageDoc.getRootElement(), "x");
            String xmlns = MRXMLUtils.getElementNameSpaceURI(eventEL, "");          

                Iterator<String> agents = this.serviceAgentFactory.getIterator();
                Document doc = MRXMLUtils.getDocument(event.getMessage());
                while (agents.hasNext()) {
                    String agent = agents.next();

                    if (this.serviceAgentFactory.getServiceAgent(agent).handleEvent(this, doc, doc.getRootElement())) {
                        notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);

                        break;
                    }
                }
*/
        } else if (eventType.equals(ServiceAgentUtils.MESSAGE) || MRClientUtils.isLinkLocalJID(event.getFrom())) { //handle all other events like AgentMessages, AgentEvent, MediaRoomInvites  and XMPPEvents
            DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(messageDoc);
            Element eventEL = dbUpdateEvent.getUpdateEventElement();//MRXMLUtils.getDescendentWithoutNamespace(messageDoc.getRootElement(), "event");            

            String xmlns = MRXMLUtils.getElementNameSpaceURI(eventEL, "");            
            //If the message is pub sub then handle the event
            if (xmlns.equals(ResponseElement.IPVSDBSERVICE)) {
                if (eventEL != null) { // Hand the event to the service agents

                    Iterator<String> agents = this.serviceAgentFactory.getIterator();
                    notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);

                    while (agents.hasNext()) {
                        String agent = agents.next();

                        if (this.serviceAgentFactory.getServiceAgent(agent).handleEvent(this, messageDoc, eventEL)) {
                            notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);

                            break;
                        }
                    }
                }
            } else {            	
                Element agentMessageElement = MRXMLUtils.getElementXpath(messageDoc, "//" + ServiceClient.AGENTMESSAGE);
                //System.out.println("agentMessageElement:="+agentMessageElement.asXML());
                 
                if(agentMessageElement != null){
                    String agentMessageType = MRXMLUtils.getAttributeValue(agentMessageElement, "type","");
                    //handle agent Started event. 
                    if (agentMessageType.equals(ServiceClient.EVENT)) {
                    	  String eventAction = MRXMLUtils.getValueXpath(messageDoc, "//action", "");

                        if (eventAction.equals(MRConst.AGENT_STATE)) {
                            handleAgentStateEvent(message, event);
                            //don't return internal messages to ui
                            return null;
                        }  else if (eventAction.equals(MRConst.AGENT_RESOURCE_STATE)) {
                            handleAgentResourceStateEvent(message, event);
                            //don't return internal messages to ui
                            return null;
                        }
                    }
                    
                    if (MRClientAgentUtils.isStale(messageDoc.getRootElement())) {
                        this.log.addInfo(this.getUserJID() + ":MRClientAgent: stale AgentMessage event dropped", MRLog.OK, MRLog.NOTIFY);

                        return MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);
                    }

                   
                    //In latest agent message service name gives us corresponding agent name
                    String agentName = ""; 

                    if(agentMessageType.equals(ServiceClient.RESPONSE)) { 
                    	//System.out.println("AGentREsponse:="+agentMessageElement.asXML());
                    	agentName =  MRXMLUtils.getValueXpath(agentMessageElement, ".//"+MRConst.CLIENT_CALLBACK, "");
                    } else {
                    	
                    	String serviceName = MRXMLUtils.getAttributeValueXpath(messageDoc,"//Header" ,"serviceName", "");                    	
                    	agentName = serviceToAgentMapping.get(serviceName);
                    	if(agentName ==null) {
                            agentName="";
                        }
                    }
                    ServiceAgentI agent = this.serviceAgentFactory.getServiceAgent(agentName.trim());
                    if (agent != null) {
                        String from = MRXMLUtils.getAttributeValue(messageDoc.getRootElement(),"from");
                        String type = MRXMLUtils.getAttributeValue(messageDoc.getRootElement(),"type");

                        if (ServiceAgentUtils.GROUPCHAT.equals(type)) {
                            StringTokenizer st = new StringTokenizer(from, "/");
                            st.nextToken();
                            fromJID = st.nextToken();

                            // For groupchat we check the agentJID to see if messge is meant for us
                            String agentJID = MRXMLUtils.getValueXpath(messageDoc, "//agentJID","");

                            if (!ServiceAgentUtils.matchesJID(this.getUserJID(), agentJID)) {
                                if (MediaRoomServiceConst.DESTSTREAMAGENT.equals(agentName)) {
                                    // print all dest messages not destinated for us
                                    String action = MRXMLUtils.getValueXpath(messageDoc, "//action", "");
                                    String streamNID = MRXMLUtils.getValueXpath(messageDoc, "//streamNID", "");

                                    if (MediaRoomServiceConst.START.equals(action)) {
                                        this.log.addInfo(this.getUserJID() + ":MRClientAgent:" + " roomMessage to=" + agentJID + ":" + agentName + "/" + streamNID + " action=" + action, MRLog.OK, MRLog.NOTIFY);
                                    }
                                }

                                return MRClientAgentUtils.createNotificationXML(message, ServiceClient.AGENTMESSAGE);
                            }
                        }

                        long t = System.currentTimeMillis();
                        // XMPPTransaction.setDEBUG(true);
                        XMPPTransaction.startBlock(agentName);
                        message = agent.handleMessage(this, fromJID, message);
                        XMPPTransaction.endBlock(agentName);
                        // XMPPTransaction.setDEBUG(false);

                        long now = System.currentTimeMillis();
                        long delay = now - t;

                        if (delay > 2000) {
                            String action = MRXMLUtils.getValueXpath(messageDoc, "//action", "unknown");
                            String eventName = MRXMLUtils.getAttributeValueXpath(messageDoc, "//Event/Header","eventName", "");
                            this.log.addInfo("MRClientAgent:=" + this.getUserJID() + " DELAY=" + delay + " agentName=" + agentName + " action=" + action
                            		+ " eventType=" + eventType + " eventName=" + eventName, MRLog.OK, MRLog.NOTIFY);
                        }

                        // Relay Support
                        String destAgentName = ServiceAgentUtils.getAgentName(MediaRoomServiceConst.DESTSTREAMAGENT, this.getUserJID());

                        if (destAgentName.equals(agentName)) {
                            // Notify the SourceStreamAgent, in case this is a relay input stream
                            String sourceAgentName = ServiceAgentUtils.getAgentName(MediaRoomServiceConst.SOURCESTREAMAGENT, this.getUserJID());
                            SourceStreamAgent sourceAgent = (SourceStreamAgent) this.serviceAgentFactory.getServiceAgent(sourceAgentName);
                            sourceAgent.handleRelayInputStreamMessage(this, fromJID, message);
                        }
                    } else if (MRClientAgent.class.getName().equals(agentName)) {
                        String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"");
                        this.log.addInfo("MRClientAgent:=" + this.getUserJID() + " > Received Response " + requestName, MRLog.OK, MRLog.NOTIFY);

                        if (requestName.equals(MRRequest.getRequestName(SetServiceDomainRequestHandler.class.getName()))) {
                            handleSetServiceDomainResponse(messageDoc);
                            log.addInfo("SetServiceDomain Completed",MRLog.INFO,MRLog.NOTIFY);
                        }

                        MRXMLUtils.setAttributeValueXPath(messageDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NID,String.valueOf(runningNumber));
                        MRXMLUtils.setAttributeValueXPath(messageDoc,MRConst.HEADER_XPATH,MRConst.USER_JID,String.valueOf(userJID));                        

                        if (MRClientAgent.LEGACYRESPONSE) {
                            Element responseElement = MRXMLUtils.getElementXpath(messageDoc, "//" + ServiceClient.RESPONSE);
                            Document responseDoc = MRXMLUtils.getDocument();
                            responseDoc.setRootElement((Element) responseElement.detach());
                            this.messageHandler.handleServiceMessage(userJID, MRXMLUtils.documentToString(responseDoc));
                        }
                    } else {
                    	// Handle the presenceEvent
                        String eventName = MRXMLUtils.getAttributeValueXpath(messageDoc, "//Event/Header","eventName", "");
                        if("MediaPresenceEvent".equals(eventName)) {
                        	Element mediaPresence = MRXMLUtils.getElementXpath(messageDoc, "//MediaPresenceEventData");
	                    	String presenceType = mediaPresence.attributeValue("type");
	                    	String roomUserJID = mediaPresence.attributeValue("jid");
	                    	String value = mediaPresence.attributeValue("value");
	                    	String roomNID = mediaPresence.attributeValue("roomNID");
	                    	boolean available = "Ready".equals(value);
	                        String destAgentName = ServiceAgentUtils.getAgentName(MediaRoomServiceConst.DESTSTREAMAGENT, this.getUserJID());
	                        DestStreamAgent destAgent = (DestStreamAgent) this.serviceAgentFactory.getServiceAgent(destAgentName);
	                        String sourceAgentName = ServiceAgentUtils.getAgentName(MediaRoomServiceConst.SOURCESTREAMAGENT, this.getUserJID());
	                        SourceStreamAgent sourceAgent = (SourceStreamAgent) this.serviceAgentFactory.getServiceAgent(sourceAgentName);
                        	
	                        if(destAgent != null) {
                                destAgent.handleMediaPresence(this, messageDoc, presenceType, value, roomUserJID, available, roomNID);
                            }
	                        if(sourceAgent != null) {
                                sourceAgent.handleMediaPresence(this, messageDoc, presenceType, value, roomUserJID, available, roomNID);
                            }
                    	}
                    }

                    /*
                    if( MRClientAgent.class.getName().equals(agentName) ||
                            AppServerAgent.class.getName().equals(agentName)        )   {
                            return null; // TBD : This is throwing off the test script
                    }
                    */
                    notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.AGENTMESSAGE);
                    notificationType = ServiceClient.AGENTMESSAGE;
                }
                else {
                	String bodyStr = MRXMLUtils.getValueXpath(messageDoc, "//body", "");
                	if(bodyStr.indexOf(XMPPI.MEDIAROOMINVITE) != -1) {
	                	Document bodyDoc = MRXMLUtils.stringToDocument(bodyStr);
	                	String roomNID = MRXMLUtils.getValueXpath(bodyDoc, "//" + XMPPI.MEDIAROOMINVITE + "/roomNID", "");
	                    if (!"".equals(roomNID)) {
	                        this.handleInvite(roomNID);
	                        notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.AGENTMESSAGE);
	                        notificationType = ServiceClient.AGENTMESSAGE;
	                    }
                	}
                }
            }
        }
        
        if (notification == null) {
            notification = MRClientAgentUtils.createNotificationXML(message, ServiceClient.XMPPEVENT);
        }

        return notification;
    }

    @Override
    public void handleResponse(MRRequest request, String logData) {
        String responseXML = ServiceClient.generateResponseXML(request, logData);

        // Send the response to the result handler
        messageHandler.handleServiceMessage(request.getUserJID(), responseXML);
    }


    

    /*
    public static void main(String[] args) throws Exception {
    	Document doc = MRXMLUtils.stringToDocument(
    			"<doc><agentmessage xmlns='abc'><agent/></agentmessage></doc>");
    	Element agentmessage = MRXMLUtils.getDescendentWithoutNamespace(doc.getRootElement(), "agentmessage");


    	// The right way to access agent
        XPath x = XPath.newInstance("//ipvs:agent");
        x.addNamespace("ipvs", "abc");
    	System.out.println(x.selectSingleNode(doc));
    	
    	// Try using agentmessage as the start node
        x = XPath.newInstance("//ipvs:agentmessage");
        x.addNamespace("ipvs", "abc");
    	agentmessage = (Element)x.selectSingleNode(doc);
    	System.out.println(MRXMLUtils.elementToString(agentmessage));
        x = XPath.newInstance("//agent");
        x.addNamespace("ipvs", "abc");
    	System.out.println(x.selectSingleNode(agentmessage));
    	
    	// Try Workaround by setting the prefix as empty
    	Document d = MRXMLUtils.stringToDocument("<x:d xmlns:x='xyz'/>");
    	d.getRootElement().add(agentmessage.detach());
        x = XPath.newInstance("//agent");
        x.addNamespace("", "abc");
    	System.out.println(x.selectSingleNode(d));
    }
    */
    /*    */
    
    /*
    public static void main(String[] args) throws Exception {
    	String xml = FileUtils.readFile("/miyer/work/mediaroom_13844/src/com/ipvs/test/t1.xml");
    	System.out.println(xml);
    	Document doc = MRXMLUtils.stringToDocument(xml);
    	System.out.println(MRXMLUtils.documentToString(doc));
    }
    */
}
