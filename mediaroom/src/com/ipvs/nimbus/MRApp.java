package com.ipvs.nimbus;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentFactory;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBUtilsBase;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.policyadminservice.impl.PolicyAdminAgent;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

/*
 * The purpose of this class is to translate the messageBus and db calls to a form close to the SC api
 * using the MRAppMessageBus and the MRAppDB classes, they will in turn call the MRComponent class methods
 */
public class MRApp implements XMPPF {
	MRAppMessageBus appMessageBus;
	MRAppDB appDB;
	MRPubSub pubSub;
	AppServerAgent appServerAgent;
	String userJID;
	MRLog mrLog;
	MRDB mrdb;
	ServiceAgentFactoryI serviceAgentFactory;
	PolicyAdminAgent policyAdminAgent;
	public static final String ADMIN = "admin";
	protected String adminPrefix;
	protected String localConfigFile = null;
	
	
	public void start(MRMessageBusI messageBus, MRDBI db) {
		MRAppLog.log("MRApp:start");
		this.appMessageBus = new MRAppMessageBus(messageBus);

		this.userJID = "managementserver.localhost";
		this.localConfigFile = "/etc/config/Device.xml";

		//TBD: we have to find how we will use OSGI logger instead of our own MRLog
		
        this.mrLog = MRLog.getInstance(MRLog.CONSOLE_DAEMON,true);
        this.mrLog.setLogLevel(MRLog.INFO);
        
		mrdb = new MRDB(this, userJID, mrLog);
		this.pubSub = new MRPubSub(appMessageBus);
		this.appDB = new MRAppDB(db, mrdb, pubSub);
		
		DBUtilsBase.theIpvsDB = this.appDB;
		DBUtilsBase.forceXMPP = true;
		this.adminPrefix = ADMIN + "@";
        this.init();
	}

	/*
	 * Initialize all XMPPF related information
	 */
	private void init() {
		try {
			MRXMLUtils.setServiceVersion();
	        this.serviceAgentFactory = new ServiceAgentFactory();
	    	this.policyAdminAgent = PolicyAdminAgent.getPolicyAdminAgent();
	    	this.appServerAgent = new MRAppServerAgent();
	    	this.serviceAgentFactory.addAgent(policyAdminAgent.getClass().getName(), policyAdminAgent);
	    	this.serviceAgentFactory.addAgent(AppServerAgent.class.getName(), appServerAgent);
	    	
	    	// Init MRDB since this is used to figure out the collectionName
		    String schemaPath = MRConst.MRSCHEMAPATH;
            Document schemadoc = MRXMLUtils.loadXMLResource(schemaPath, mrLog);
            String serviceDomain = "default";
	        String ipvsRootNID = "MEDIAROOM.SERVICE.DOMAIN." + serviceDomain;        
            mrdb.init(ipvsRootNID, serviceDomain, schemadoc);
            this.appDB.initDB(0, mrdb, this.mrLog, ipvsRootNID, serviceDomain, schemaPath, false);
            
            // Init the XMPPI
            this.appMessageBus.setSuperUser(MRServer.ADMIN);

			AppServerAgent.setLocalConfigFile(localConfigFile);
            // Init AppServer so that it can start all the managers
			AppServerAgent.setXmppEnable(true);
	    	this.appServerAgent.init(this.appMessageBus, this.mrLog, mrdb, null, serviceAgentFactory, this.userJID, null);
	    	
	    	// Init the PolicyAdmin agent
	    	this.policyAdminAgent.init(this, this.mrLog, mrdb, null, this.userJID, null);
	    	
	    	this.appServerAgent.handleReady();	    	
	    	
	    }
	    catch(Exception e) {
	    	e.printStackTrace();
	    }
		try {
			String appServerAgentJID = AppServerAgent.readAppServerInfoFromDB(this.appMessageBus, this.mrLog, mrdb, null, serviceAgentFactory, userJID);
			MRAppLog.log("MRApp:init appServerAgentJID=" + appServerAgentJID);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	
	public void stop() {
		MRAppLog.log("MRApp:stop");
		this.appMessageBus = null;
		this.appDB = null;
		try {
			this.appServerAgent.reset();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		this.appServerAgent = null;
	}
	
	public MRRequestHandlerI getHandler(String serviceName, String requestName) throws Exception {
		String handlerPackage = "com.ipvs." + serviceName + ".handler";
		String handlerName = handlerPackage + "." + requestName + "Handler";
		try {
			Class<?> c = Class.forName(handlerName);
			return (MRRequestHandlerI)c.newInstance();
		}
		catch(Exception e) {
			throw new MRException(MRExceptionCodes.REQUESTNOTFOUND, "Handler not found for request=" + requestName);
		}
	}

	public void handleMessage(MRSCMessage message) throws MRSCException, Exception{
		try {
			this.handleMessageInternal(message);
		}
		catch(MRException me) {
			throw new MRSCException(me.getCode(), me.getData(), me, null);
		}
	}
	
	// Generate policy request doc from cache
	
    // MIYE Added since we are no longer receiving db updates in MRComponent
    public void checkForDBEvent(String serviceName, Document requestDoc, Document responseDoc) {
    	try {
	    	String requestTag = null;
	        if(serviceName.equals(ServiceClient.ASSETADMINSERVICE)){
	        	requestTag = "AssetManagementRequestData";
	        }
	        else if(serviceName.equals(ServiceClient.POLICYADMINSERVICE)){
	        	requestTag = "PolicyManagementRequestData";
	        }
	        else
	        	return;

	        // String s = requestDoc.asXML();
	        if(MRXMLUtils.getElementXpath(requestDoc, "//" + requestTag + "/GetRequest") != null) // is a GetRequest
	        	return; 
	        
	        String objectType = MRXMLUtils.getValueXpath(responseDoc, "//ObjectType", "");
	        if(objectType == null)
	        	return;
	        
	        // objPEMDBUtils.setPolicyDirtyFlag(objectType, true);            
	        PolicyManager.handlePolicyChanged(objectType, serviceAgentFactory);
    	}
    	catch(Exception e) {
			MRAppLog.log("PolicyAdminAgent:checkForDBEvent checkForDBEventAppServer error=" + e.toString());
    	}
    }

	public void handleMessageInternal(MRSCMessage message) throws MRSCException, Exception {
		// Invoke the handler and get the response
		// MRAppLog.log("MRApp:handleMessage");
		Document doc = (Document)message.getMessageDoc().clone();
		String serviceName = message.getService();
		String rootName = message.getRootName();
		if("sessionservice".equals(serviceName)) {
			serviceName = SessionServiceUtils.getServiceName(rootName + "Request", this.mrLog);
		}
		String resourceGroup = null;
		String clientData = message.getClientData();
		String appServerJID = this.userJID;
		String userAgentName = message.getClientCallback();
		int logLevel = message.getLogLevel().toInt();
		
		String messageType = message.getType();
		if(MRSCMessage.REQUEST.equals(messageType)) {
			String requestNID = userJID + "/" + String.valueOf(System.currentTimeMillis());
			Element reqEl = MRXMLUtils.getElementXpath(doc, "//"+MRSCMessage.REQUEST);
			MRXMLUtils.removeNodeXpath(doc, "//AgentMessage");
			MRXMLUtils.addElement(doc, reqEl);
			MRMessage mrMessage = new MRMessage(doc);
			String requestName = rootName + "Request";
			MRRequest request = new MRRequest(message.getFrom(), userAgentName, appServerJID, logLevel, requestNID, 
					serviceName, requestName, mrMessage, resourceGroup,clientData);
			MRRequestHandlerI handler = this.getHandler(serviceName, requestName);
			// String requestXML = request.getMessage().getDocument().asXML();
			if(message.getFrom().startsWith(this.adminPrefix)){
				request.setSuperUser(true);
			}
			Document response = handler.getResponse(request, this.appMessageBus, this.mrLog, this.mrdb, null, this.serviceAgentFactory);
			if(response != null)
				message.setResult(response.asXML());
			
			this.checkForDBEvent(serviceName, doc, response);
		}
		else if(MRSCMessage.RESPONSE.equals(messageType)) {
			this.appServerAgent.processAgentResponseMessage(doc);			
		}
		else if(MRSCMessage.EVENT.equals(messageType)) {
	        this.appServerAgent.processAgentEventMessage(doc);
		}
		else {
			MRAppLog.log("AppServerAgent::handleServiceMessage:=" +  message.getFrom() + " invalid messagetype=" + messageType);
		}
	}

	@Override
	public XMPPI xmpp(String userJID) throws Exception {
		return this.appMessageBus;
	}

	@Override
	public ServiceAgentFactoryI sf(String userJID) throws Exception {
		return this.serviceAgentFactory;
	}

}
