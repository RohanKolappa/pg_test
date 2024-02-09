package com.ipvs.mediaroomservice.impl;

import java.io.File;
import java.net.InetAddress;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.jboss.resteasy.plugins.server.servlet.HttpServletDispatcher;
import org.jivesoftware.smack.packet.IQ.Type;
import org.jivesoftware.smack.packet.XMPPError;

import com.ipvs.app.http.servlet.MediaDownloadServlet;
import com.ipvs.app.http.servlet.MediaImageServlet;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.appserver.ServiceConfigManager;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.AgentResponseHandlerThread;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRService;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.CommonThreadPoolManager;
import com.ipvs.common.service.ResourcePresenceManager;
import com.ipvs.common.service.UserManager;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.device.jetty.GetURLServlet;
import com.ipvs.http.server.IPVSHttpServer;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.handler.HandleUnmanagedDestinationStreams;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.notification.ResourceManager;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.DeviceTypeList;
import com.ipvs.sessionservice.impl.AutoStartManager;
import com.ipvs.sessionservice.impl.PVRManager;
import com.ipvs.sessionservice.impl.ReportManager;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.ScheduleManager;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.ClearDownloadCacheRequestHandler;
import com.ipvs.sessionservice.resource.handler.CreateMediaFromCutListRequestHandler;
import com.ipvs.sessionservice.resource.handler.HttpResourceRequestHandler;
import com.ipvs.sessionservice.resource.handler.PrepareFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.RegisterUserRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.TrimFileRequestHandler;
import com.ipvs.sessionservice.service.handler.AuditEventRequestHandler;
import com.ipvs.sessionservice.service.handler.GetServiceClientRequestHandler;
import com.ipvs.sessionservice.service.handler.KickoutRequestHandler;
import com.ipvs.sessionservice.service.handler.LogoutRequestHandler;
import com.ipvs.sessionservice.service.handler.NotificationChannelServlet;
import com.ipvs.sessionservice.service.handler.ServiceLoginRequestHandler;
import com.ipvs.sessionservice.session.handler.AuthenticateURLRequestHandler;
import com.ipvs.sessionservice.session.handler.HttpSessionRequestHandler;
import com.ipvs.sessionservice.session.handler.ThumbnailStreamRequestHandler;
import com.ipvs.subscriptions.SubscriptionManager;
import com.ipvs.systemservice.handler.RoomGC;
import com.ipvs.systemservice.handler.SetServicePolicyRequestHandler;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.ProducesBarcoJsonInterceptor;
import com.ipvs.xmpp.client.DBUpdateEvent;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;
import com.ipvs.xmpp.client.smack.packets.SessionIQ;


public class AppServerAgent implements  ServiceAgentI, MRResponseHandlerI, XMPPF {
	
	public static final String ASSETRESOURCE_OWNER_XPATH = "//Contact/OwnerUserJID";
	public static final String ASSETRESOURCE_OWNERSTATE_XPATH = "//State/StateFlag";
	public static final String SERVICELOGIN_XPATH = "//ServiceLogin";
	private static final String SET_SERVICE_POLICY_REQUEST_XML = "/com/ipvs/systemservice/xml/SetServicePolicyRequest.xml";
	private static final String SET_SERVICE_POLICY_REQUEST_NAME = "SetServicePolicyRequest";
	
    private static final java.lang.String APP_SERVER_READY_STATUS = "/var/status/appservice.ready";
	
    private XMPPI xmpp;
    protected MRLog log;
    protected MRDB db;
    private MRResourceFactoryI resourceFactory;
    private ServiceAgentFactoryI serviceAgentFactory;
    private PVRManager thePVRManager = null;
    protected String userJID;    
	private static String msAssetNID = null;
	private static String localConfigFile = null;

	private String appServerAgentJID = null;
	private static String testAppServerAgentJID = null;		
	private static boolean xmppEnable = false;	
	private MRServer server;
	private UserManager userManager = UserManager.getInstance();	
	private SubscriptionManager subscriptionManager = null;
	private ResourceManager resourceManager = null;
	private SessionManager sessionManager = null;
	private ScheduleManager scheduleManager = null;
	private AuthTokenCache authTokenCache  = null;
	private ResourcePresenceManager resourcePresenceManager = null; 
	private ServiceConfigManager serviceConfigManager = null;
	private CommonThreadPoolManager threadPoolManager=null;
	
	private Element apiKeyTableEl =null;
	private IPVSHttpServer httpServer = null;

	private AutoStartManager theAutoStartManager = null;
	private RoomGC roomGC = null;
	protected void intializeMRServer(MRResponseHandlerI responseHandler) throws Exception {

		this.server = new MRServer(xmpp, log, db,  responseHandler, resourceFactory, serviceAgentFactory);	 	
		this.server.addService(new MRService(ServiceClient.SYSTEMSERVICE));
		this.server.addService(new MRService(ServiceClient.MEDIAROOMSERVICE));
		this.server.addService(new MRService(ServiceClient.ASSETADMINSERVICE));
		this.server.addService(new MRService(ServiceClient.ASSETADMINSERVICE_DEVICE));
		this.server.addService(new MRService(ServiceClient.ASSETADMINSERVICE_PORT));
		this.server.addService(new MRService(ServiceClient.ASSETADMINSERVICE_FILE));
		this.server.addService(new MRService(ServiceClient.ASSETADMINSERVICE_DIR));
		this.server.addService(new MRService(ServiceClient.POLICYADMINSERVICE));
		this.server.addService(new MRService(ServiceClient.SESSIONSERVICE));
		this.server.addService(new MRService(ServiceClient.SESSIONSERVICE_RESOURCE));
	    this.server.addService(new MRService(ServiceClient.SESSIONSERVICE_SERVICE ));
		this.server.addService(new MRService(ServiceClient.SESSIONSERVICE_SESSION));
		this.server.addService(new MRService(ServiceClient.SESSIONSERVICE_WINDOW));
		
		log.addInfo("AppserverAgent: Initialize attribute lookup table", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor.initAttributeLookupTable();
    	
	}

	protected void startHttpServer() throws Exception {
		httpServer = new IPVSHttpServer(10088, log);
		ServletContextHandler servletContextHandler = new ServletContextHandler(httpServer, "/controller", true, false);
		
		servletContextHandler.addServlet(new ServletHolder(new MediaImageServlet(this,  log,  db,  resourceFactory,
				userJID)), "/snapshot");
		servletContextHandler.addServlet(new ServletHolder(new MediaDownloadServlet(this,  log,  db,  resourceFactory,
				userJID)), "/File/Download");
		
		servletContextHandler.addServlet(new ServletHolder(new GetURLServlet(xmpp,  log,  db,  resourceFactory,
				serviceAgentFactory,  userJID)), "/session");
		
		ServletHolder servletDispatcher = new ServletHolder(HttpServletDispatcher.class);		
	
		//Instead of restreasy.scan we are using resteasy.resources for this release.
		//Once we have created structures for web-inf etc we will use resteasy.scan 
		servletDispatcher.setInitParameter("resteasy.resources",
				HttpResourceRequestHandler.class.getName() + ","
						+ HttpSessionRequestHandler.class.getName() + ","
						+ ServiceLoginRequestHandler.class.getName() + ","
						+ LogoutRequestHandler.class.getName() + ","
						+ KickoutRequestHandler.class.getName() + ","
//						+ NotificationChannelRequestHandler.class.getName() + ","
						+ AuditEventRequestHandler.class.getName() + ","
						+  ThumbnailStreamRequestHandler.class.getName() + "," 
						+ GetServiceClientRequestHandler.class.getName() + ","
						+ PrepareFileRequestHandler.class.getName() + ","
						+ CreateMediaFromCutListRequestHandler.class.getName()+ ","
						+ ClearDownloadCacheRequestHandler.class.getName()+ ","
						+ TrimFileRequestHandler.class.getName()+ ","
						+ AuthenticateURLRequestHandler.class.getName()+ ","
						+ RegisterUserRequestHandler.class.getName()); 
		//add notification channel servlet
		servletContextHandler.addServlet(new ServletHolder(new NotificationChannelServlet(this,  log,  db,  resourceFactory,
				userJID)), "/Service/NotificationChannel");
		
		servletDispatcher.setInitParameter("resteasy.providers", ProducesBarcoJsonInterceptor.class.getName());
		servletContextHandler.addServlet(servletDispatcher , "/*");	

		// TBD: Adding xmpp, resourceFactory etc. to the context handler so they can be accessed in the servletcontext.
		servletContextHandler.setAttribute("ServiceAgentFactoryI", serviceAgentFactory);
		servletContextHandler.setAttribute("XMPPI", xmpp);
		servletContextHandler.setAttribute("MRLog", log);
		servletContextHandler.setAttribute("MRDB", db);
		servletContextHandler.setAttribute("MRResourceFactoryI", resourceFactory);
		httpServer.startServer();
	}

    @Override
    public boolean handleEvent(MRClient client, Document doc, Element event)
            throws Exception {
    	return false;
    }
    
    public void handlePersistentPVRSessionRestart(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
    		ServiceAgentFactoryI serviceAgentFactory, String mediaDirNID, boolean restartSession) throws Exception {    	
    	thePVRManager.handleDirStateUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID, restartSession);
    }
    
    public boolean isServiceResourceUpdateEvent(String message,MRLog log,MRDB db,String userJID)throws Exception{    	
    	Document messageDoc = MRXMLUtils.stringToDocument(message);
        DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(messageDoc);
        if(dbUpdateEvent.getUpdateEventElement() == null) {
            return false;         
        }        

    	String parentID = dbUpdateEvent.getSubscribedNID();
    	MRConfig config = MRConfig.getInstance(AMConst.SERVICE_RESOURCE,log);    	
    	String serviceResourceParentID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
    	if(!parentID.equals(serviceResourceParentID)){
    		return false;
    	}    	
		log.addInfo("AppServerAgent:isServiceResourceUpdateEvent Checking resourceUpdate", MRLog.OK, MRLog.NOTIFY);
    	if (dbUpdateEvent.isAddorUpdate()) {
            String serviceResourceNID = dbUpdateEvent.getUpdateNID();
            if (!isXMPPEnabled(userJID)) {
                //on first time login after factory... the service resource may not be there in db yet.. resulting in msAssetNID to be empty .
                //on update event -- it can be read again to get non empty value.. 
                if (msAssetNID == null || msAssetNID.equals("")) {
                    //System.out.println("******** Login first time after factory. Re-Read Service Resource **************");
                    log.addInfo("Login first time after factory. Re-Read Service Resource", MRLog.INFO, MRLog.NOTIFY);
                    readAppServerInfoFromDB(db.getXMPPI(), log, db, null, null, userJID);
                }
                if (serviceResourceNID.equals(msAssetNID)) {
                    log.addInfo("AppServerAgent:isServiceResourceUpdateEvent Detected resourceUpdate", MRLog.OK, MRLog.NOTIFY);
                    return true;
                }
            }
            log.addInfo("AppServerAgent:isServiceResourceUpdateEvent Ignored resourceUpdate", MRLog.OK, MRLog.NOTIFY);
        } 
    	return false;
    }

    @Override
    public String handleMessage(MRClient client, String fromJID, String message)
        throws Exception {
    	Document doc = MRXMLUtils.stringToDocument(message);
    	String messagetype  = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(doc, "//AgentMessage"), "type", "");
    	String clientCallBack = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_CALLBACK, "");    	
    	Document requestDoc = null;
    	if(ServiceClient.REQUEST.equals(messagetype)) {
    		Element requestElement = MRXMLUtils.getElementXpath(doc, "//" + ServiceClient.REQUEST);
    		requestDoc = MRXMLUtils.getDocument();
    		requestDoc.setRootElement((Element)requestElement.detach());
    		String resourceGroup = ""; // TBR
    		try {
				if (server == null) {
				   throw new MRException(MRExceptionCodes.SERVICENOTAVAILABLE, "<error>" + "Login not found for sendRequest" + "</error>");
				}
				// TBD: the fromJID here is different from MRClientAgent.userJID
				// XMPP returns "localhost" as the severName in the fromJID
				// mediaroom code uses "IP" as the serverName
				// fromJID = fromJID.replace("localhost", client.getServerName());
				this.server.handleRequest(fromJID, clientCallBack, this.userJID, resourceGroup, requestDoc, this);
			} catch (MRException e) {
				handleFailureRequest(fromJID, requestDoc, clientCallBack, e);
				e.printStackTrace();
			} catch(Exception e){
				handleFailureRequest(fromJID, requestDoc, clientCallBack, new MRException(MRExceptionCodes.HANDLE_REQUEST_FAILED,e.toString()));
				e.printStackTrace();				
			}
    	}
    	else if (ServiceClient.RESPONSE.equals(messagetype)) {
            this.log.addInfo("AppServerAgent::handleServiceMessage:=" + this.userJID + " received Response from userJID=" + fromJID, MRLog.OK, MRLog.NOTIFY);
           
           processAgentResponseMessage(doc);
            
    	}
    	else if (ServiceClient.EVENT.equals(messagetype)) {
            this.log.addInfo("AppServerAgent::handleServiceMessage:=" + this.userJID + " received AgentEvent from userJID=" + fromJID, MRLog.OK, MRLog.NOTIFY);
    	   // XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
    	   XMPPTransaction.startBlock("processAgentEventMessage");
           processAgentEventMessage(doc);
    	   XMPPTransaction.endBlock("processAgentEventMessage");
   		   // XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
        } else {
            this.log.addInfo("AppServerAgent::handleServiceMessage:=" + this.userJID + " invalid messagetype=" + messagetype, MRLog.OK, MRLog.NOTIFY);
        }
        return message;
    }
    
    private void handleFailureRequest(String userJID,Document requestDoc,String fromAgentName,MRException e)  throws MRException,Exception {
    	MRRequest request = MRRequest.createRequest(userJID, 0, "", "", MRXMLUtils.getAttributeValueXpath(requestDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, ""), new MRMessage(requestDoc), "",getAppServerAgentJID(this.userJID),fromAgentName,"");
		request.setState(MRRequest.STATE_EXCEPTION);
		request.setException(e);
		this.handleResponse(request, "");
    }

    @Override
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc) throws Exception{
    	this.init(xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID), userJID, setServiceDomainResponseDoc);
    }
    
    public void init(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID, Document setServiceDomainResponseDoc) throws Exception{
        this.xmpp = xmpp;
        this.log = log;
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.serviceAgentFactory = serviceAgentFactory;
        this.userJID = userJID;    	
        int counter = 0;
        final int MAX_RETRY_COUNT = 20;
        
        if(isXMPPEnabled(this.userJID)) {
        	HAUtils.updateHAProperties(log);
			this.appServerAgentJID = userJID;	
			intializeMRServer(this);
			initializeManagers(this); 			
			initSetServicePolicy();  
			//init service manager after policies and profiles initialized.
			serviceConfigManager.init();
        	
        	setAPIKey();
        	        	
        	log.addInfo("AppserverAgent: Initializing report scheduler", MRLog.OK, MRLog.NOTIFY);
    		try {
				ReportManager.getInstance().initializeReportScheduler(xmpp, log, db, resourceFactory, serviceAgentFactory, 
						this.appServerAgentJID, userJID);
			} catch (Exception e1) {
				e1.printStackTrace();
				log.addInfo("AppServerAgent: Unable to initialize report scheduler" + e1.toString());
			}
        	
        	try {
				resourcePresenceManager.updateResourceListStateAsOffline(userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);
			} catch (Exception e1) {
				e1.printStackTrace();
				log.addInfo("AppServerAgent: Unable to reset resource states " + e1.toString());
			}
        	ArrayList<String> roomList = xmpp.getRoomNIDList();
            System.out.println("AppServerAgent GettingRoomList #Count=" + roomList.size());
            Iterator<String> i = roomList.iterator();
        	while(i.hasNext()) {
        		
        		String roomNID = i.next();
        		System.out.println("Found RoomNID=" + roomNID);
        		try {
        			// xmpp.joinRoom(roomNID, ServiceAgentUtils.getRoomNickname(userJID));
        			System.out.println("AppServerAgent Calling delete room");
        			if(AppServerAgent.testAppServerAgentJID == null) {
                        MediaRoomServiceUtils.deleteMediaRoom(xmpp, db, log, serviceAgentFactory, resourceFactory, roomNID, userJID);
                    }
        		}catch(Exception e) {
        			e.printStackTrace();
        			log.addInfo("AppServerAgent Unable to delete room" + e.toString());
        		}
        	}
        } else {
        	if(AppServerAgent.testAppServerAgentJID == null) {
        		while((this.appServerAgentJID == null) && (counter < MAX_RETRY_COUNT)){ // Added to enable testing
        			log.addInfo("Read AppServer info from db" ,MRLog.INFO,MRLog.NOTIFY);              
        			this.appServerAgentJID = AppServerAgent.readAppServerInfoFromDB(xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
        			if(this.appServerAgentJID != null) {
        				break;
        			}
        			
        			counter++;
        			log.addInfo("Retrying for AppServerAgentJID" ,MRLog.INFO,MRLog.NOTIFY);                	
        		}        		
        	}
        }
        
        System.out.println("NIGHTLY TESTING - setservicedomain issue with xmpp_server_status.exp");
        log.addInfo("NIGHTLY TESTING - setservicedomain issue with xmpp_server_status.exp",MRLog.INFO,MRLog.NOTIFY);
        
        
		if(this.appServerAgentJID == null) {
			log.addInfo("NIGHTLY TESTING - setservicedomain issue with xmpp_server_status.exp",MRLog.INFO,MRLog.NOTIFY);
			throw new MRException(MediaRoomServiceExceptionCodes.APPSERVER_JID_NOT_FOUND,"App Server JID Not Found");
		}					
		log.addInfo("AppserverAgent:  AppServerAgentJID="+this.appServerAgentJID ,MRLog.INFO,MRLog.NOTIFY);
		ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), this.getClass().getName(), MediaStoreServiceConst.READY, this.xmpp, this.log);
    }   


	private void initializeManagers(AppServerAgent appServerAgent) {
		serviceConfigManager = new ServiceConfigManager(log, this, db, userJID);
		thePVRManager = new PVRManager();   
		theAutoStartManager = new AutoStartManager();
		subscriptionManager = new SubscriptionManager(log, this, db, userJID);
		authTokenCache = new AuthTokenCache(db, log, xmpp, resourceFactory, serviceAgentFactory);
		roomGC = new RoomGC(db, log, xmpp, resourceFactory, serviceAgentFactory);
    	resourceManager = new ResourceManager(db,log,this, userJID,subscriptionManager,authTokenCache);  
    	sessionManager = new SessionManager(db,log, this, userJID, resourceManager,authTokenCache);
    	scheduleManager = new ScheduleManager();
    	resourcePresenceManager = new ResourcePresenceManager();
    	threadPoolManager = CommonThreadPoolManager.getInstance();    	
	}

	public static boolean isXMPPEnabled(String userJID) throws MRException,Exception  {    	
    	if(userJID != null && userJID.equals(testAppServerAgentJID)) {
            return true;
        }
    	return xmppEnable;
    }
    
    public static void setLocalConfigFile(String localConfigFile)
    {
    	AppServerAgent.localConfigFile = localConfigFile;
    }
    
    public PVRManager getPVRManager() {
    	return thePVRManager;
    }
    
    public AutoStartManager getAutoStartManager() {
    	return theAutoStartManager;
    }
    public void sendDeleteMediaRoomRequest(String roomNID) throws Exception {

        String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.DELETEMEDIAROOM_REQUEST_XML;
        Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
        MRXMLUtils.getElementXpath(requestDoc, "//roomGC").setText(MRConst.TRUE);

        String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
        String agentClassName = AppServerAgent.class.getName();
        String fromAgentClassName = AppServerAgent.class.getName();
        String info = "AppServerAgent::handleOwnerUnavailable > Sending DeleteMediaRoomRequest";

        String requestNID = "none";
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, 
        		fromAgentClassName, null, agentJID, agentClassName, requestDoc, info, requestNID);
    }
    
    public void sendCleanMonitorAgentMediaStreamsRequest(String roomNID, String monitorAgentJID) throws Exception {
    	
    	String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.CLEANMONITORAGENTSTREAMS_REQUEST_XML;
    	Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
        MRXMLUtils.getElementXpath(requestDoc, "//MonitorAgentJID").setText(monitorAgentJID);
        
        String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
        String agentClassName = AppServerAgent.class.getName();
        String fromAgentClassName = AppServerAgent.class.getName();
        String info = "AppServerAgent::sendCleanMonitorAgentMediaStreamsRequest roomNID=" + roomNID;
        
        String requestNID = "none";
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, 
        		fromAgentClassName, null, agentJID, agentClassName, requestDoc, info, requestNID);
                
    }

    ///////////////////////////
    // Presence related methods
    ///////////////////////////
    
    private void handleStreamEntityPresence(Element streamElement, boolean available, String roomNID, MRRequest request) throws Exception {
		String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
		log.addInfo("AppServerAgent:handleStreamEntityPresence streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);

		int state = MediaRoomServiceConst.STREAM_STOPPED;
		try { 
			state = Integer.parseInt(MRXMLUtils.getValue(MRXMLUtils.getElement(streamElement,"Status"),"state",""));
		}
		catch(Exception e) { e.printStackTrace();}
		if(available) {
			// System.out.println("AppServerAgent:handlePresence available roomUserJID=" + roomUserJID + " streamstate=" + state);
			if(state == MediaRoomServiceConst.WAITING_FOR_DEST) {
				String profileNID = streamElement.element("data").element("profileNID").getText();
				Element profileXML = streamElement.element("data").element("profileXML");
				log.addInfo("AppServerAgent:handleStreamEntityPresence start streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
				ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, "AddStreamActionRequest", 
						roomNID, streamNID, profileNID,  profileXML ,MediaRoomServiceConst.START);
			}
		} else {
			if ( (state != MediaRoomServiceConst.SETUP_ERROR) && (state != MediaRoomServiceConst.STREAM_STOPPED) &&
					(state != MediaRoomServiceConst.WAITING_FOR_XAUTH) &&
					(state != MediaRoomServiceConst.WAITING_FOR_CONTACTJID) ) {
				log.addInfo("AppServerAgent:handleStreamEntityPresence stop streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
				MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
	        		String.valueOf(MediaRoomServiceConst.WAITING_FOR_DEST));
			}
		}
    }

    private boolean handleRoomPresence(MRClient client, Document doc, Element event, String roomUserJID, boolean available, String roomNID,
    		Element assetResourceElement)
    throws Exception {    	
	    MRRequest request = MRRequest.createRequest(userJID);
		// if(!MediaRoomServiceUtils.objectExists(roomNID, request, db, log, roomNID)) {
		if(!RoomDB.getRoomDB().roomExists(roomNID)) {
			return false;
		}
		
        Document roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		String presence = MediaRoomServiceConst.UNAVAILABLE;
		if(available) {
			presence = MediaRoomServiceConst.AVAILABLE; 
		}

		// If unavailable and is mediaclient delete Rooms owned by the user 
		String assetType = MRXMLUtils.getValueXpath(assetResourceElement, ".//Type", "");
		// String xml = assetResourceElement.asXML();
        String roomOwnerJID = MRXMLUtils.getValueXpath(roomDoc, "//roomOwnerJID", "");
        if(!available && 
        		!(AMConst.WEB_CLIENT_DEVICE.equals(assetType) || AMConst.PROXY_CLIENT_DEVICE.equals(assetType))
        		&& roomUserJID.equals(roomOwnerJID)) {
        	log.addInfo("AppServerAgent:handleRoomPresence deleteMediaRoom userJID="+roomUserJID + " assetType=" + assetType,MRLog.INFO,MRLog.NOTIFY);
        	this.sendDeleteMediaRoomRequest(roomNID);
        	return true;
        }
        else {
        	log.addInfo("AppServerAgent:handleRoomPresence skipping deleteMediaRoom userJID=" + roomUserJID + " roomOwnerJID:="+ roomOwnerJID + " assetType=" + assetType,MRLog.INFO,MRLog.NOTIFY);
        }

		// Update the MediaSource if matches
		String xpath  = "//MediaSource [data/sourceAgentJID = '" + roomUserJID + "']";	    
    	List<?> mediaSourceList = MRXMLUtils.getListXpath(roomDoc, xpath);
    	// System.out.println("AppServerAgent:handlePresence userJID=" + userJIDWithMSIP + " presence=" + presence +
    	//		" count=" + mediaSourceList.size());
    	for (Object name : mediaSourceList) {
    		Element mediaSourceElement = (Element)name;
    		String mediaSourceNID = MRXMLUtils.getAttributeValue(mediaSourceElement,"NID");
			MediaRoomServiceUtils.updatePresence(xmpp, request, log, db, roomNID, mediaSourceNID, presence);
    	}
    	
		// Update the MediaRelay if matches
		xpath  = "//MediaRelay [data/relayAgentJID = '" + roomUserJID + "']";	    
    	List<?> mediaRelayList = MRXMLUtils.getListXpath(roomDoc, xpath);
    	// System.out.println("AppServerAgent:handlePresence userJID=" + userJIDWithMSIP + " presence=" + presence +
    	//		" count=" + mediaSourceList.size());
    	for (Object name : mediaRelayList) {
    		Element mediaRelayElement = (Element)name;
    		String mediaRelayNID = MRXMLUtils.getAttributeValue(mediaRelayElement,"NID");
			MediaRoomServiceUtils.updatePresence(xmpp, request, log, db, roomNID, mediaRelayNID, presence);
    	}
    	
		// Update the MediaDest if matches
		xpath  = "//MediaDest [data/destAgentJID = '" + roomUserJID + "']";	    
    	List<?> mediaDestList = MRXMLUtils.getListXpath(roomDoc, xpath);
    	for (Object name : mediaDestList) {
    		Element mediaDestElement = (Element)name;
    		String mediaDestNID = MRXMLUtils.getAttributeValue(mediaDestElement,"NID");
			MediaRoomServiceUtils.updatePresence(xmpp, request, log, db, roomNID, mediaDestNID, presence);
	        // Check if we need to restart the stream
	    	List<?> streamElementList = MRXMLUtils.getListXpath(roomDoc, "//Stream [data/mediaDestNID = '" + mediaDestNID + "']");
	    	// System.out.println("AppServerAgent:handlePresence mediaDestNID=" + mediaDestNID + " roomdoc=" + MRXMLUtils.documentToString(roomDoc) );
	    	for(Object o:streamElementList) {
	    		Element streamElement = (Element)o;
	    		this.handleStreamEntityPresence(streamElement, available, roomNID, request);
	    	}
    	}
    	
	    return true;
    }
    
    public void handleDeviceUnavailable(String userJID) throws Exception {
    	this.handlePresenceUnavailable(null, null, null, userJID);
    }

    private boolean handlePresenceUnavailable(MRClient client, Document doc, Element event, String userJID) throws Exception {
    	
    	String contactJID = userJID;   	
		
    	Element responseEl = resourcePresenceManager.updateResourceContactAndState(contactJID,true, AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log,xmpp,resourceFactory,serviceAgentFactory);
    	userManager.removeJID(contactJID);
    	
    	subscriptionManager.removeUserFromSubscriberPolicyCache(contactJID);
    	//unsubscribe the userJID while logout
    	MRXMLDBUtils.unsubscribe(MRRequest.createRequest(contactJID), db, log,"", "");
    	
    	Element assetResourceElement = MRXMLUtils.getChild(responseEl, "AssetResource");
        if(assetResourceElement == null) {
        	return false;
        }
    	
    	MRRequest request = MRRequest.createRequest(userJID);
    	Hashtable<String, String> updateNidMap = new Hashtable<String, String>();
    	//stop the session on encoders that are configured to autostart    	
    	//String portNID = MRXMLUtils.getValueXpath(responseEl, ".//NIDValue", "");
    	List<String> portNIDList = MRXMLUtils.getValuesXpath(responseEl, ".//NIDValue");
    	for (String portNID : portNIDList) {
    	    if(portNID!= null && !portNID.isEmpty() ){  		
    		request.setSuperUser(true);
    		
    		//theAutoStartManager.handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.STOP_AUTOSTART_SESSION);
    		
    		updateNidMap.put(portNID, "Update");

    		// queue port object notifications.
    		getResourcePresenceManager().addPortObjectNotificationsToQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, updateNidMap);
    	    }
    	}

    	log.addInfo("AppServerAgent:handlePresenceUnavailable UserJID:="+userJID,MRLog.INFO,MRLog.NOTIFY);
    	ArrayList<String> roomNIDList = RoomDB.getRoomDB().handleAssetUnavailable(xmpp, log, MRXMLUtils.getAttributeValue(assetResourceElement,"NID",""), userJID);
    
    	//log logout event
    	AuditEvent auditEvent =  new AuditEvent.Builder(AuditEventLogger.SYSTEM_EVENT, AuditEventLogger.NAME_LOGOUT, contactJID, AuditEventLogger.LEVEL_INFO)
		.status(AuditEventLogger.STATUS_SUCCESS).build();
    	AuditEventLogger.getInstance().addAuditEventToQueue(auditEvent);
    	
    	
    	String whereXPath = "//MediaRoom[//MediaSource/data[monitorAgentJID='"+ userJID + "'] and //data[roomType != '" + MediaRoomServiceConst.RELAY_CONNECTION + "']]";
    	// String whereXPath = "//MediaRoom[(//MediaSource/data[monitorAgentJID='"+ userJID + "'] and //data[roomType != '" + MediaRoomServiceConst.RELAY_CONNECTION + "') or //data[roomOwnerJID='"+ userJID + "']";
    	boolean includeObserverData = true; 
    	Document roomDoc = RoomDB.getRoomDB().getMediaRoomListWithFilter(request, xmpp, log, db, whereXPath, includeObserverData, roomNIDList);
    			
    	List<?> roomList = MRXMLUtils.getListXpath(roomDoc, "//MediaRoom");
    	for (Object name : roomList) {
    		Element roomElement = (Element)name;
    		String roomNID = MRXMLUtils.getAttributeValue(roomElement, "NID");
    		sendCleanMonitorAgentMediaStreamsRequest(roomNID, userJID);
        	log.addInfo("AppServerAgent:handlePresenceUnavailable sendCleanRoom roomNID="+roomNID,MRLog.INFO,MRLog.NOTIFY);
    	}    	
    	
    	for(String roomNID : roomNIDList) {
    		this.handleRoomPresence(client, doc, event, userJID, false, roomNID, assetResourceElement);
    	}
        return false;
    }
    
    public boolean handlePresence(MRClient client, Document doc, Element event)
            throws Exception {
    	
    	if (event.getName().compareToIgnoreCase(ServiceAgentUtils.PRESENCE) == 0) {
    		if (!AppServerAgent.isXMPPEnabled(this.userJID)) {
                return false;
            }
    		String userJID = MRXMLUtils.getAttributeValue(event, "from", null);
    		String presenceType = MRXMLUtils.getAttributeValue(event, "type", null);            
    		if ((presenceType != null) && (presenceType.compareToIgnoreCase("unavailable") == 0)) {
    			handlePresenceUnavailable(client,doc, event, userJID);
    		}
    	}    	
    	return false;
    }
 

    private void initSetServicePolicy() throws Exception
    {
    	MRRequest mrRequest = MRRequest.createRequest(userJID);
    	
    	new SetServicePolicyRequestHandler().addDeviceInDB(null, mrRequest, xmpp, log, db,resourceFactory, serviceAgentFactory, PolicyAdminConstants.ASSET_RESOURCE_OBJECT, DeviceTypeList.valueOf("WebClient"));
    	
    	MRConfig mrConfig = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, PolicyAdminConstants.USER, log);
        String parentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));	
        String[] whereXPath = {"//" + PolicyAdminConstants.USER + "[" + PolicyAdminConstants.USER_NAME + "='" + SystemServiceConst.ADMIN + "']"};
        
    	Element userEL = MRXMLDBUtils.readElement(0, null, whereXPath, parentNID, null, mrRequest, log, db);    		
        if (userEL != null) {
            return;
        }
        log.addInfo("Admin User XML Not there in database. Trigger default policy Setup",MRLog.INFO,MRLog.NOTIFY);
        String type = PolicyAdminConstants.ALLOW_ALL;        	
    	String bandwidth = "10000";
    	String relay = "false";

    	String deviceType = "";
    	    	
    	if (localConfigFile != null && !localConfigFile.isEmpty())
    	{
    		Document deviceDoc = MRXMLUtils.getDocument(new File(localConfigFile));
    		deviceType = deviceDoc.getRootElement().getName();
    	}
    	
    	if (deviceType.contains(SystemServiceConst.DMS_OBJECT_NAME)) {
            relay = "true";
        }        	
    	
    	Document setServicePolicyRequestDoc = MRXMLUtils.loadXMLResource(SET_SERVICE_POLICY_REQUEST_XML, log);
    	MRXMLUtils.setValueXpath(setServicePolicyRequestDoc, "//" + PolicyAdminConstants.TYPE, type);        	        	
    	MRXMLUtils.setValueXpath(setServicePolicyRequestDoc, "//" + AMConst.BANDWIDTH, bandwidth);
    	MRXMLUtils.setValueXpath(setServicePolicyRequestDoc, "//" + PolicyAdminConstants.RELAY, relay);

		MRRequest request = MRRequest.createRequest(mrRequest, setServicePolicyRequestDoc, SET_SERVICE_POLICY_REQUEST_NAME, ServiceClient.SYSTEMSERVICE);
		
		SetServicePolicyRequestHandler handler = new SetServicePolicyRequestHandler();
		handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }
   

    // Send the response to the result handler
    private void sendAgentResponseMessage(MRRequest request, String logData, String info) throws Exception {
    	String responseXML = ServiceClient.generateResponseXML(request, logData);
    	//log.addInfo("responseXML::"+responseXML);
	    ServiceAgentUtils.sendAgentMessage(xmpp, log, 
	    		AppServerAgent.class.getName(), 
	    		null, 
	    		request.getUserJID(),
	    		request.getUserAgentName(),
	    		ServiceClient.RESPONSE, 
	    		responseXML, 
	    		info);
    }
    
    
    private void sendIQResponse(String toJID, String id, 
    		String childElementXML, int state, String errorXML, String info) throws Exception {
		SessionIQ sessionElement = new SessionIQ();
		sessionElement.setFrom(this.userJID);
		sessionElement.setPacketID(id);
		sessionElement.setTo(toJID);
		sessionElement.setType(Type.RESULT);
		sessionElement.setChildElementXML(childElementXML);			
		if(state == MRRequest.STATE_EXCEPTION) {
			sessionElement.setType(Type.ERROR);
			IPVSMessageExtension extension = new IPVSMessageExtension(
					IPVSMessageExtension.IPVS_SESSION_XMLNS);
			extension.setXML(errorXML);
			XMPPError ipvsXMPPError = new XMPPError(XMPPError.Condition.bad_request);
			ipvsXMPPError.addExtension(extension);
			sessionElement.setError(ipvsXMPPError);
		}
		// log.addInfo("AppServerAgent:sendIQResponse userJID=" + toJID + " iq=" + sessionElement.toXML(), MRLog.INFO,MRLog.NOTIFY);
		log.addInfo(info, MRLog.INFO,MRLog.NOTIFY);
		this.xmpp.sendPacket(sessionElement);
    }

	
    @Override
    public void handleResponse(MRRequest request, String logData) {
        try {
        	
        	//if request contains audit event list log the events
        	for(AuditEvent auditEventFromRequest = null;(auditEventFromRequest=request.removeAuditEvent())!=null;){
        		AuditEventLogger.getInstance().addAuditEventToQueue(auditEventFromRequest);
        	}
        	
    		String stateStr = String.valueOf(request.getState());
    		if(request.getState() == MRRequest.STATE_EXCEPTION) {
    			stateStr = stateStr +  "." + String.valueOf(request.getException().getCode());
    		}
    	    String info = "AppServerAgent > Response:" + request.getRequestName() + ":" + 
    	    	stateStr + ":" + request.getDuration() + " to " + request.getUserAgentName();
    	   
    	    AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).addResultAndResumeContinuation(request, logData, stateStr);
        	
    	    if(ServiceClient.IPVS_SESSION_AGENT.equals(request.getUserAgentName())) {
            	String childXML = SessionServiceUtils.responseToQueryXML(request);
            	String errorXML = null;
        		if(request.getState() == MRRequest.STATE_EXCEPTION) {
        			errorXML = SessionServiceUtils.exceptionToIQErrorXML(request.getException());
        		}
            	this.sendIQResponse(request.getUserJID(), 
            			request.getClientData(), 
            			childXML, 
            			request.getState(), 
            			errorXML, info);

        	}
        	else {
        		this.sendAgentResponseMessage(request, logData, info);
        	}
        }
        catch(Exception e) {
        	e.printStackTrace();
        }
    }
    
    public void processAgentResponseMessage(Document messageDoc) {
        Document responseDoc = null;
        String serviceName = null;

        try {
            String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
            if (requestName.isEmpty()) {
                return;
            }
            
            serviceName = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, "");;
            if(serviceName.isEmpty()){
            	return;
            }

            responseDoc = MRXMLUtils.getDocumentXpath(messageDoc, "//" + ServiceClient.RESPONSE);
            Document userRequestDoc = null;
            String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
            clientData = URLDecoder.decode(clientData, "UTF-8");
            // System.out.println("AppServerAgent: Parsing clientData=" + clientData);
            try {
            	userRequestDoc = MRXMLUtils.stringToDocument(clientData);
            }
            catch(Exception e) {
            	// Not a user doc
            }
            if(userRequestDoc == null)
             {
                return; // Not an agentResponse
            }


            String responseName = MRRequest.getResponseName(requestName);
           
            String requestNID = MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NID, "");
            
            AgentResponseHandlerThread responseHandlerThread = new AgentResponseHandlerThread();
            //Thread handlerThread = new Thread(responseHandlerThread);
            Document requestDoc = MRXMLUtils.getDocument();
            MRRequest request = MRRequest.createRequest(this.userJID, MRLog.INFO, requestNID, serviceName, requestName, new MRMessage(requestDoc), "",  AppServerAgentUtils.getAppServerAgentJID(this.userJID,serviceAgentFactory));
            request.setResult(responseDoc);
            
            String responseHandlerName = MRXMLUtils.getValueXpath(userRequestDoc, "//responseHandlerName", "");

            responseHandlerThread.init(responseName, serviceName, request, this, xmpp, db, log, resourceFactory, serviceAgentFactory,
            		responseHandlerName);
            
           // handlerThread.start();
            getThreadPoolManager().getAgentResponseFixedPool().execute(responseHandlerThread);
        } catch (MRException exp) {
            exp.printStackTrace();
        } catch (Exception exp) {
        	exp.printStackTrace();
        }
    }

    public void processAgentEventMessage(Document messageDoc) throws Exception {
        Document eventDoc = MRXMLUtils.getDocumentXpath(messageDoc, "//" + ServiceClient.EVENT);
        try {
            String eventName = MRXMLUtils.getAttributeValueXpath(eventDoc, "//Event/Header","eventName", "");
            String eventWC = MRXMLUtils.getAttributeValueXpath(eventDoc, "//Event/Header","eventWallclock", "");
            if (eventName.isEmpty()) {
                return;
            }                  
            if(DestStreamAgent.STREAMSTATUSEVENT.equals(eventName)) {
            	Element streamStatusElement = MRXMLUtils.getElementXpath(eventDoc, "//" + 
            			DestStreamAgent.STREAMSTATUSEVENT + "Data");
            	String streamNID = MRXMLUtils.getAttributeValue(streamStatusElement,"streamNID");
            	String roomNID = MRXMLUtils.getAttributeValue(streamStatusElement,"roomNID");
            	String state = MRXMLUtils.getValueXpath(eventDoc,"//StreamStatusEventData/state","");
            	
            	streamStatusElement.addAttribute("streamNID",null);
            	streamStatusElement.addAttribute("roomNID",null);
            	streamStatusElement.addAttribute("eventWallcolck",eventWC);
            	
            	MRRequest request = MRRequest.createRequest(userJID);
         	    XMPPTransaction.startBlock("objectExists");
         	    boolean roomExists = RoomDB.getRoomDB().roomExists(roomNID);
         	    XMPPTransaction.endBlock("objectExists");
        		if(!roomExists) {
        			return; // Room no longer present
        		}
         	    XMPPTransaction.startBlock("updateStreamStatus");
         	    //TODO: Review with iyer regarding  lock on stream nid
         	    log.addInfo("AppserverAgent :Handle Stream Status Event aquire lock on  StreamNID:="+streamNID +" State:="+state);
         		Semaphore lock = IPVSMonitor.getMonitor().getLock(streamNID, request.getNID());				
         		log.addInfo("AppserverAgent :Handle Stream Status Event aquired lock on  StreamNID:="+streamNID +" State:="+state);
           	 
            	try {
            		log.addInfo("AppserverAgent :Handle Stream Status Event eventWallclock=" + eventWC + " RoomNID:="+roomNID+" StreamNID:="+streamNID +" State:="+state);
            	    
            		MediaRoomServiceUtils.updateStreamStatus(this.xmpp, request, db, log, roomNID, streamNID, streamStatusElement);
            	} catch (MRException e) {
            		if(e.getCode() == MediaRoomServiceExceptionCodes.ROOMNOTFOUND) {
            			// might have been deleted
            		}
            		else {
	            		e.printStackTrace();
	            		System.out.println("AppServerAgent:processAgentEventMessage Failed for agentEvent=" + MRXMLUtils.documentToString(eventDoc));
            		}
            	} catch (Exception e) {
            		e.printStackTrace();
            		System.out.println("AppServerAgent:processAgentEventMessage Failed for agentEvent=" + MRXMLUtils.documentToString(eventDoc));
            	}finally  {
            		log.addInfo("AppserverAgent :Handle Stream Status Event release lock on  StreamNID:="+streamNID +" State:="+state);
            		IPVSMonitor.getMonitor().releaseLock(streamNID, lock);
            	}
         	    XMPPTransaction.endBlock("updateStreamStatus");
            } else  if (MediaRoomServiceConst.SOURCEEVENT.equals(eventName)) {            	
            	new HandleUnmanagedDestinationStreams().handleSourceEvent(xmpp,  db, log, serviceAgentFactory, resourceFactory, eventDoc);
            } else  if(MediaRoomServiceConst.RELAYEVENT.equals(eventName)){
            	new HandleUnmanagedDestinationStreams().handleRelayEvent(xmpp, db, log, serviceAgentFactory, resourceFactory, eventDoc);
            }else  if (MediaRoomServiceConst.MEDIA_PRESENCE_EVENT.equals(eventName)) {            	
            	new HandleUnmanagedDestinationStreams().handleMediaPresenceEvent(xmpp, db, log, serviceAgentFactory, resourceFactory, eventDoc);
            }
            
            //TODO:handle source events here
        } catch (MRException exp) {
            exp.printStackTrace();
        } catch (Exception exp) {
        	exp.printStackTrace();
        }
    }
    
    public static String readAppServerInfoFromDB(XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
    		String userJID) throws Exception {    	
    	Element serviceResourceElement = null;

    	MRRequest surequest = MRRequest.createRequest(userJID);   
    	MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_RESOURCE, log);
        String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));              	            	    									
    
      	String whereXPath[] = {"//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE+ "']"};
    	serviceResourceElement = MRXMLDBUtils.readElement(1, null, whereXPath, rootNID, 1, null, null, surequest, db, log);    
    	
    	if(serviceResourceElement == null) {
    		return null;
    	}
    	
    	msAssetNID = MRXMLUtils.getAttributeValue(serviceResourceElement, "NID", "");	
    	String agentJID = MRXMLUtils.getValueXpath(serviceResourceElement, AMConst.CONTACT+"/"+AMConst.OWNERUSERJID, null);
    	
    	if("".equals(agentJID)) {
    		return null;
    	}
    	return agentJID;
	}	

	

	public String getAppServerAgentJID(String userJID) throws MRException,Exception {
		if(AppServerAgent.testAppServerAgentJID != null) {
			return AppServerAgent.testAppServerAgentJID;
		} 
		if(this.appServerAgentJID == null) {
			//return userJID;	
			throw new MRException(MediaRoomServiceExceptionCodes.APPSERVER_JID_NOT_FOUND,"App Server JID Not Found");
		}
		return this.appServerAgentJID;
	}	

	public static String getTestAppServerAgentJID() {
		return testAppServerAgentJID;
	}

	public static void setTestAppServerAgentJID(String testAppServerAgentJID) {
		AppServerAgent.testAppServerAgentJID = testAppServerAgentJID;
	}

	public void handleReady() throws Exception {
	    
		//start HTTP server
		startHttpServer();
		
		//update resource state
		updateAppServerInfoInDB(xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);

		//add discovery feature
		xmpp.addDiscoFeature("BarcoSCService");
		
		//update ready status file		
		FileUtils.writeFile(APP_SERVER_READY_STATUS, "true");
		
	}
	
	public  void updateAppServerInfoInDB(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID) throws Exception {
		MRRequest surequest;
		surequest = MRRequest.createRequest(userJID);
		surequest.setSuperUser(true);
		String whereXPath = "//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE + "']";
	
		Document servicedoc = getServiceDoc(xmpp, log, db, resourceFactory, serviceAgentFactory, surequest,whereXPath);
		Element serviceResourceElement = MRXMLUtils.getElementXpath(servicedoc, "//ServiceResource");
		if(serviceResourceElement!=null) {
			String msAssetNID = MRXMLUtils.getAttributeValue(serviceResourceElement,"NID");
			MRXMLUtils.setValueXpath(serviceResourceElement, AMConst.CONTACT+"/"+AMConst.OWNERUSERJID, userJID);
            HAUtils.updateServiceResourceWithHA(servicedoc, userJID, db, log);
            log.addInfo("AppServerAgent: Update ServiceResource. AppserverAgentJID:="+userJID  , MRLog.INFO,MRLog.NOTIFY);
			MRXMLDBUtils.updateElement(surequest, log, db, msAssetNID, serviceResourceElement);
			
			//do extra update for legacy releases. 
			//String message = "<x><request xmlns='ipvs:mediaroom:ipvsdbservice'><replace   domain='default'  nodeID='"+msAssetNID+"'  "
			//		+ "subscribeJID='"+appServerAgentJID+"'  DBCallTimeOut='300000'  ><data>"+serviceResourceElement.asXML()+"</data></replace></request></x>";
			//xmpp.sendMessage("ipvsdbservice.localhost", message, "ipvs:mediaroom:ipvsdbservice");
		}
		else {			
			addServiceResourceInDB(xmpp,log,db,resourceFactory,serviceAgentFactory, userJID,"Ready",true,false);
		}			
		
	
		log.addInfo("AppServerAgent: Write Ready status file:="+APP_SERVER_READY_STATUS, MRLog.INFO,MRLog.NOTIFY);

	}
	public void addServiceResourceInDB(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID, String stateFlag, boolean isUpdateHAEnabled, boolean isAddApiKeyTableEnabled)throws Exception{
		String xmlFilePath = "ServiceResource_BASE_ALL_ADD.xml";
		Document msgdoc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);
		MRXMLUtils.setValueXpath(msgdoc, "//OwnerUserJID", userJID);
		MRXMLUtils.setValueXpath(msgdoc, "//StateFlag", stateFlag);
		MRXMLUtils.setValueXpath(msgdoc, "//Type", AMConst.APP_SERVICE_TYPE);
		MRXMLUtils.setValueXpath(msgdoc, "//Description", "App Service");
		MRRequest surequest = MRRequest.createRequest(userJID);
		surequest.setSuperUser(true);
        log.addInfo("AppServerAgent: Update ServiceResource. AppserverAgentJID:="+userJID , MRLog.INFO,MRLog.NOTIFY);
        if(isUpdateHAEnabled)
        	HAUtils.updateServiceResourceWithHA(msgdoc, userJID, db, log);
        if(isAddApiKeyTableEnabled)
        	addApiKeyTabletoServiceResourceElement(surequest,log, db, MRXMLUtils.getElementXpath(msgdoc, "//ServiceResource"));
		MediaRoomServiceUtils.getAssetAdminResponse(surequest, xmpp, log, db, resourceFactory, serviceAgentFactory, "assetadminservice", "AssetManagementRequest", msgdoc);
		
		log.addInfo("AppServerAgent: Add ServiceResource. AppserverAgentJID:="+userJID, MRLog.INFO,MRLog.NOTIFY);
	}
	public Document getServiceDoc(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRRequest request, String whereXPath) throws Exception
	{
		Element whereAssistXML = MRXMLUtils.stringToElement("<ServiceResource></ServiceResource>");
		//String whereXPath = "//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE + "']";
		Document servicedoc = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "assetadminservice", "ServiceResource_BASE_ALL_GET.xml",
				whereAssistXML, whereXPath);

		
		return servicedoc;
	}

/*	public static String getLocalConfigFile() {
		return localConfigFile;
	}

	public static void setLocalConfigFile(String localConfigFile) {
		AppServerAgent.localConfigFile = localConfigFile;
	}
*/

    public static void sendAgentRequestForUserRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String fromAgentName, String toAgentJID, String serviceName, String dataStr,
    		String responseHandlerName)
	    throws Exception {
       //String serviceName = request.getServiceName();
        String requestName = request.getRequestName();
        sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, requestName, serviceName, dataStr, responseHandlerName);
	}
    
    
    public static void sendAgentRequestForUserRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String fromAgentName, String toAgentJID, String agentRequestName,String agentServiceName,String dataStr,
    		String responseHandlerName)
	    throws Exception {
    	
        String info = request.getRequestName() + "::sendAgentRequestForUserRequest " + 
        	fromAgentName + " to " + agentServiceName;
        Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, agentServiceName, agentRequestName, dataStr,
        		info, request.getNID(), request.getUserJID(), agentServiceName, clientData);
	}
    
    public static void sendDeprecatedAgentRequestForUserRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String fromAgentName, String toAgentJID, String agentRequestName,String agentServiceName,String dataStr,
    		String responseHandlerName, String deprecated)
	    throws Exception {
        
        String info = request.getRequestName() + "::sendAgentRequestForUserRequest " + 
        	fromAgentName + " to " + agentServiceName;
        Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
        ServiceAgentUtils.sendDeprecatedAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, agentServiceName, agentRequestName, dataStr,
        		info, request.getNID(), request.getUserJID(), agentServiceName, clientData, deprecated);
	}

	public static void setXmppEnable(boolean xmppEnable) {
		AppServerAgent.xmppEnable = xmppEnable;
	}
		
	public void handleSessionIQ(String fromJID, Element query, String id) throws Exception {
		// System.out.println("TBD : AppServerAgent:handleSessionIQ");
		
    	String fromAgentName = ServiceClient.IPVS_SESSION_AGENT;
    	if(this.server == null) {
    		this.log.addInfo("AppServerAgent:handleSessionIQ no MRServer found", MRLog.OK, MRLog.NOTIFY);
    		return;
    	}
    	
		try {
			Document requestDoc = SessionServiceUtils.QueryToSessionServiceRequestDoc(fromJID, query, id, log);
			if(!DocumentHelper.createXPath(SERVICELOGIN_XPATH).booleanValueOf(requestDoc)) {
				authorizeRequest(fromJID);
			}
			String resourceGroup = ""; // TBR
			this.server.handleRequest(fromJID, fromAgentName, this.userJID, resourceGroup, requestDoc, this);
		}
		catch(MRException mre) {
			String errorXML = SessionServiceUtils.exceptionToIQErrorXML(mre);
    	    String info = "AppServerAgent > Response: " + errorXML + " to " + fromJID;
            log.addInfo(info , MRLog.ERROR, MRLog.NOTIFY);

			this.sendIQResponse(fromJID, id, MRXMLUtils.elementToString(query), MRRequest.STATE_EXCEPTION, errorXML, info);
		}
		catch(Exception e) {
			MRException ex = new MRException(MRExceptionCodes.INVALIDREQUEST, "");
			String errorXML = SessionServiceUtils.exceptionToIQErrorXML(ex);
    	    String info = "AppServerAgent > Response: " + errorXML + " to " + fromJID;
            log.addInfo(info , MRLog.ERROR, MRLog.NOTIFY);

			this.sendIQResponse(fromJID, id, MRXMLUtils.elementToString(query), MRRequest.STATE_EXCEPTION, errorXML, info);
		}
	}

	private void authorizeRequest(String fromJID) throws Exception {
	    
	        
		if(StringUtils.isEmpty(fromJID)) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					"JID is null or empty");
		}
		
		String resource = org.jivesoftware.smack.util.StringUtils.parseResource(fromJID);
		if(StringUtils.isEmpty(resource)) {
			throw new MRException(MRExceptionCodes.RESOURCE_AUTHERIZATION_FAILED,
					"Resource value is null or empty in JID. ");
		}
		
		//if the user is admin no need to check authorization
		String user = org.jivesoftware.smack.util.StringUtils.parseName(fromJID);
		if(user.equals(MRServer.ADMIN)) {
		    return;
		}
		
		//Document resourceDoc = MRClientUtils.readAssetResourceInternal(userJID, log, db, resource);//does caching
		userManager.checkJID(fromJID, log, db, resource);
		
	}
	
	public static String getMsAssetNID() {
		return msAssetNID;
	}
	
	
	public SubscriptionManager getSubscriptionManager() {
		return subscriptionManager;
	}


	@Override
    public void reset() throws Exception {
		resetManagers();
		resetMRServer();
		resetHttpServer();
		appServerAgentJID = null;
	}

	private void resetMRServer() {
		this.server = null;		
	}

	private void resetHttpServer() {
		try {
			if(httpServer !=null){
				httpServer.stopServer(); 
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void resetManagers() {
		if(!xmppEnable) {
            return;
        }
		thePVRManager = null;
		sessionManager.reset();
		sessionManager = null;
		resourceManager.reset();
		resourceManager = null;
		subscriptionManager = null;
		authTokenCache.reset();
		authTokenCache = null;
		theAutoStartManager = null;
		roomGC = null;
		
	}

	/**
	 * @return the notificationManager
	 */
	public ResourceManager getResourceManager() {
		return resourceManager;
	}

	/**
	 * @return the sessionManager
	 */
	public SessionManager getSessionManager() {
		return sessionManager;
	}
	/**
	 * @return the NotificationThreadPoolManager
	 */
	public CommonThreadPoolManager getThreadPoolManager() {
		return threadPoolManager;
	}
	protected String getIPAddress() throws Exception {
		return DeviceAppUtils.getIPAddress(null);
	}
	
	public void setAPIKey() throws Exception {		
		/*
        <ApiKeyTable>
        	<ApiKey key="" sharedSecret="" validInterval="" ipAddress=""></ApiKey>
        </ApiKeyTable>
		 */
		//read from apikey from db, if not present create one and save in the service resource		
		MRRequest surequest = MRRequest.createRequest(userJID);
		surequest.setSuperUser(true);
		String whereXPath = "//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE + "']"; 

		Document servicedoc = getServiceDoc(xmpp, log, db, resourceFactory, serviceAgentFactory, surequest,whereXPath);
		Element serviceResourceElement = MRXMLUtils.getElementXpath(servicedoc, "//ServiceResource");
		Element apiKeyElement;
		if(serviceResourceElement!=null) {	
			String msAssetNID = MRXMLUtils.getAttributeValue(serviceResourceElement, "NID", "");
			Element apiKeyTableElement = MRXMLUtils.getElementXpath(servicedoc, "//ServiceResource/ApiKeyTable");

			if(apiKeyTableElement == null){			
				apiKeyElement = addApiKeyTabletoServiceResourceElement(surequest,log, db, serviceResourceElement);
				updateServiceResourceApiKey(surequest, log, db, serviceResourceElement, apiKeyElement);
				 MRXMLDBUtils.updateElement(surequest, log, db, msAssetNID, serviceResourceElement);
			}
			else{
				apiKeyElement = MRXMLUtils.getElementXpath(apiKeyTableElement, "//ApiKey");

				String apikey = MRXMLUtils.getAttributeValue(apiKeyElement,"key");			
				if("".equals(apikey.trim())){			
					updateServiceResourceApiKey(surequest, log, db, serviceResourceElement, apiKeyElement);
				}
				else{
					try {						
						MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.MS_IPADDRESS, DeviceAppUtils.getIPAddress(null));
					} catch (Exception e) {

						MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.MS_IPADDRESS, InetAddress.getLocalHost().getHostAddress());			
					}
					 MRXMLDBUtils.updateElement(surequest, log, db, msAssetNID, serviceResourceElement);
				}
				MRXMLDBUtils.updateElement(surequest, log, db, msAssetNID, serviceResourceElement);
				apiKeyTableEl = MRXMLUtils.getElementXpath(servicedoc, "//ServiceResource/ApiKeyTable");

			}
		}
		else{			
			addServiceResourceInDB(xmpp, log, db, resourceFactory, serviceAgentFactory, userJID, "", false,true);
		}

	}
	public Element addApiKeyTabletoServiceResourceElement(MRRequest request, MRLog log, MRDB db, Element serviceResourceElement)throws Exception{
		Element apiTableEle = MRXMLUtils.getElementXpath(serviceResourceElement, "./"+SessionServiceResourceConst.API_KEY_TABLE);
		if(apiTableEle != null)
			apiTableEle.detach();
		apiKeyTableEl= MRXMLUtils.newElement(SessionServiceResourceConst.API_KEY_TABLE);
		MRXMLUtils.addElement(apiKeyTableEl, SessionServiceResourceConst.API_KEY_ELEMENT_NAME);
		MRXMLUtils.addElement(serviceResourceElement, apiKeyTableEl);
		
		Element apiKeyElement = MRXMLUtils.getElementXpath(apiKeyTableEl, "./"+SessionServiceResourceConst.API_KEY_ELEMENT_NAME);
		updateServiceResourceApiKey(request, log, db, serviceResourceElement, apiKeyElement);
		return apiKeyElement;
	}
	public void updateServiceResourceApiKey(MRRequest request, MRLog log, MRDB db, Element serviceResourceElement, Element apiKeyElement) throws Exception{
		String apikey = ""+ new SecureRandom().nextLong();
        String secretKey = ""+ new SecureRandom().nextLong();		        
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.KEY_ATTRIBUTE, apikey);
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE, secretKey);
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE,String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
        try {
			MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.MS_IPADDRESS, DeviceAppUtils.getIPAddress(null));
		} catch (Exception e) {
			
			MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.MS_IPADDRESS, InetAddress.getLocalHost().getHostAddress());			
		}
       
	}
	public Element getAPIKey() {
		return apiKeyTableEl;
	}
	
	public ScheduleManager getScheduleManager() {
		return scheduleManager;
	}

	public AuthTokenCache getAuthTokenCache() {
		return authTokenCache;
	}
	
	public ServiceConfigManager getServiceConfigManager() {
		return serviceConfigManager;
	}

	public ResourcePresenceManager getResourcePresenceManager() {
		return resourcePresenceManager;
	}
	public RoomGC getRoomGC() {
		return roomGC;
	}
	
	@Override
	public XMPPI xmpp(String userJID) throws Exception {
		return this.xmpp;
	}

	@Override
    public ServiceAgentFactoryI sf(String userJID) throws Exception {
		return this.serviceAgentFactory;
	}

}

