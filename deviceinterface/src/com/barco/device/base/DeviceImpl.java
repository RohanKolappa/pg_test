package com.barco.device.base;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Enumeration;

import org.apache.log4j.Appender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.AgentImpl;
import com.barco.device.agent.DeviceAdminAgentI;
import com.barco.device.agent.LayoutAgentI;
import com.barco.device.agent.MediaAgentI;
import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.LoginInfo.LoginInfoEvents;
import com.barco.device.common.DeviceDBI;
import com.barco.device.common.Log4JErrorHandler;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.http.HTTPMessageBus;
import com.barco.device.xmpp.XMPPMessageBus;
import com.barco.utils.XMLUtils;

class ResponseData {
	String clientData;
	Document context;
	String clientCallback;
	ResponseHandlerI responseHandler;
	static ResponseData parseClientData(String clientData,String clientCallback) throws Exception {
		// TBD Generate the values for the context and reponsseHandler and restore clientData
		ResponseData data = new ResponseData();
		data.clientData = clientData;
		data.clientCallback = clientCallback;
		String className = clientCallback;
		Class<?> c = Class.forName(className);
		data.responseHandler = (ResponseHandlerI)c.newInstance();
		return data;
	}
	static String generateClientData(String clientData, Document context) {
		
		// TBD Serialize the context and responseHandler, clientData and update clientData
		return clientData;
	}
}

/**
 * Dispatches incoming bus-messages (request, response, event) to the
 * appropriate handler callback (reflection based handler instantiation), 
 * thereby passing along the DeviceI context
 * (which carries references to each of the AgentI's and DeviceDBI).
 * 
 * Provides a delaying construct around sending bus notifications (queue/sendNotification)
 * and helper method to send a request-message.
 *  
 * Exposes login/logout, and is a LoginInfoObserver, so reacts to LoginInfoEvents.
 */
public class DeviceImpl implements DeviceI,LoginInfoObserver {
	XMPPMessageBus xmpp;
	HTTPMessageBus http;
	LayoutAgentI layoutAgent;
	MediaAgentI mediaAgent;
	DeviceAdminAgentI deviceAdminAgent;
	MediaStoreAgentI mediaStoreAgent;
	Logger logger;
	int httpPort;
	String handlerPackage;
	public static final int DefaultHTTPPort = 80;
	public static final String DefaultHandlerPackage = "com.barco.device.handler";
	//StreamCache streamCache = new StreamCache();
	//String appserverJID = null;
	
	private String serviceVersion = null;
	private DeviceDBI deviceDB = null;

	// Logger Categories
	public static final String CONSOLE="CONSOLE";
	public static final String DAEMON="DAEMON"; // Syslog
	public static final String CONSOLE_DAEMON="CONSOLE_DAEMON";

	//set LOGGER_CATEGORY to CONSOLE_DAEMON as default
	private static String LOGGER_CATEGORY = CONSOLE_DAEMON;
	private LoginInfo loginInfo;
	private LoginDaemon loginDaemon;
	private Notifier notifier;
	
	public DeviceImpl(){}
	
	public DeviceImpl(	
			AgentImpl agentImpl,
			int httpPort,
			String handlerPackage,String appserverJID,String loginStatusFile) {
		initLogger();
		this.xmpp = new XMPPMessageBus();
		this.xmpp.init(this, this.logger);	
		loginDaemon = new LoginDaemon(this.xmpp,logger);
		loginInfo = new LoginInfo(this,logger);
		loginInfo.setAppserverJID(appserverJID);
		loginInfo.setLoginStatusFile(loginStatusFile);

		//Start HTTP server
		if(agentImpl.runHttpService) {
			this.http = new HTTPMessageBus();		
			this.http.init(this,logger);
			this.httpPort = httpPort;
			try {
				this.http.startServer(this.httpPort, "/deviceinterface");
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		this.handlerPackage = handlerPackage;
		this.deviceAdminAgent = agentImpl.deviceAdminAgent;
		this.layoutAgent = agentImpl.layoutAgent;
		this.mediaAgent = agentImpl.mediaAgent;
		this.mediaStoreAgent = agentImpl.mediaStoreAgent;
		this.handlerPackage = handlerPackage;
		this.deviceDB = agentImpl.deviceDB;
		this.notifier = new Notifier(this);
		
		initAgents();
	}

	
	private void initAgents() {
		this.deviceAdminAgent.initAgent(this);
		
		if(mediaAgent!=null) {
			this.mediaAgent.initAgent(this);
		}
		
		if(layoutAgent!=null){
			this.layoutAgent.initAgent(this);
		}
		
		if(mediaStoreAgent!=null){
			this.mediaStoreAgent.initAgent(this);
		}
		
		this.deviceAdminAgent.initAgent(this);
		
		
	}
	
	private void resetAgents() throws Exception {
		
		this.deviceAdminAgent.reset();
	
	if(mediaAgent!=null) {
		this.mediaAgent.reset();
	}
	
	if(layoutAgent!=null){
		this.layoutAgent.reset();
	}
	
	if(mediaStoreAgent!=null){
		this.mediaStoreAgent.reset();
	}
	
	this.deviceAdminAgent.reset();
	
	
}


	public void handleSystemCommand(String s) throws Exception {
		if(s.startsWith("<start"))
			this.http.startServer(httpPort, "/device");
		else if(s.startsWith("<stop"))
			this.http.stopServer();
		else
			xmpp.handleRequest(s);
	}
	
	@Override
	public void handleException(ServiceException exception) {
		this.logger.error(exception.toString(), exception);
		if(exception.getCode() == ServiceExceptionCodes.TERMINATED) {
			
			Element shutdownEventEl = DocumentHelper.createElement(DeviceAdminConst.SC_SHUTDOWN_EVENT);
			try {
				Message eventMessage = Message.createEvent(shutdownEventEl,getMyJID() , getMyJID(),
						DeviceAdminConst.DEVICE_ADMIN_SERVICE, DeviceAdminConst.SC_SHUTDOWN_EVENT, String.valueOf(Level.INFO), Level.INFO);
				this.handleMessage(eventMessage);
			} catch (Exception e1) {
			}
		}
	}

	private void handleEventInternal(Message message) {
		this.messageTrace(message.getRootName() + " from=" + message.getFrom() + " begin", message, null);
		long startTime = System.currentTimeMillis();
		
		// An Event might need to be handled by all services/agents
		ArrayList<String> serviceList = new ArrayList<String>();
		if("*".equals(message.getService())) {
			serviceList.add("Layout");
			serviceList.add("Media");			
			// serviceList.add("DeviceAdmin");
		}
		else
			serviceList.add(message.getService());

		for(String serviceName : serviceList) {
			String handlerName = handlerPackage + "." + serviceName + "." + message.getRootName() + "Handler";
			try {
				Class<?> c = Class.forName(handlerName);
				MessageHandlerI handler = (MessageHandlerI)c.newInstance();
				handler.handleMessage(message, this);
			}
			catch(Exception e) {
				e.printStackTrace();
				this.logger.error(e.toString(), e);
			}
		}
        long duration = System.currentTimeMillis() - startTime;
		this.messageTrace(message.getRootName() + " from=" + message.getFrom() +
				" duration=" + duration, message, null);		
	}
	
	
	private void handleRequestInternal(Message message) {
		this.messageTrace(message.getRootName() + " from=" + message.getFrom() + " begin", message, null);
		long startTime = System.currentTimeMillis();		
		try {
			String handlerName = handlerPackage + "." + message.getService() + "." + message.getRootName() + "Handler";
			Class<?> c = Class.forName(handlerName);
			MessageHandlerI handler = (MessageHandlerI)c.newInstance();
			handler.handleMessage(message, this);
			if(!message.isAsynch()) {
				message.setState(Message.REQUEST_STATE_DONE);
			} else {
				message.setState(Message.REQUEST_WAITING_FOR_ASYN_RESPONSE);
			}
			System.out.println("DeviceImple.handleRequestInternal: DONE");
		}
		catch(ServiceException se) {
		    System.out.println("DeviceImple.handleRequestInternal: Service Exception");
			message.setState(Message.REQUEST_STATE_ERROR);
			se.printStackTrace();			
			try {message.setResult(XMLUtils.stringToDocument(se.toString()));}
			catch(Exception e) {this.logger.error(e.toString(), e);}
		}
		catch(Exception e) {
		    System.out.println("DeviceImple.handleRequestInternal: Exception");
		    this.logger.debug(e.toString(), e);
			e.printStackTrace();
			try {message.setResult(XMLUtils.stringToDocument(new ServiceException(ServiceExceptionCodes.SYSTEMERROR, e.toString()).toString()));}
			catch(Exception e1) {this.logger.error(e1.toString(), e1);}
		}
        long duration = System.currentTimeMillis() - startTime;
		this.messageTrace(message.getRootName() + " from=" + message.getFrom() +
				" duration=" + duration, message, null);	
		System.out.println("DeviceImple.handleRequestInternal: END");
	}


	public String exceptionToString(Exception e) {
        try {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            return  e.toString() + sw.toString()  ;
        } catch (Exception e2) {
            return "";
        }
    }

	private void handleResponseInternal(Message message, Document context, ResponseHandlerI responseHandler) {
		String prefix = responseHandler.getClass().getSimpleName() + " " + message.getRootName() + " " + Message.RESPONSE + " from=" + message.getFrom();
		this.logger.info(prefix + " begin");
		long startTime = System.currentTimeMillis();
		try {
			Element headerEl  = XMLUtils.getElementXpath(message.getMessageDoc(), "//Header");
			String state = XMLUtils.getAttributeValueWithDefault(headerEl, "state", "0");
			Element ipvserror  = XMLUtils.getElementXpath(message.getMessageDoc(), "//ipvserror");
			
			
			if(ipvserror != null || state.equals("0")) {
				responseHandler.handleError(message, context, this);
			}
			else					
				responseHandler.handleResponse(message, context, this);
		}
		catch(ServiceException se) {
			try {message.setResult(XMLUtils.stringToDocument(se.toString()));}
			catch(Exception e) {this.logger.error(e.toString(), e);}
		}
		catch(Exception e) {
			e.printStackTrace();
			this.logger.error(this.exceptionToString(e), e);
		}
        long duration = System.currentTimeMillis() - startTime;
		this.logger.info(prefix + " duration=" + duration);
	}

	@Override
	public void handleMessage(Message message) throws Exception {

		this.logger.info("DeviceImpl: handleMessage: " + message.getRootName() + " type= " + message.getType() + " from= " + message.getFrom() + " to= " + message.getTo() + " service= "+ message.getService());
		if(Message.RESPONSE.equals(message.getType())) {
			ResponseData responseData = ResponseData.parseClientData(message.getClientData(),message.getClientCallback());
			if(responseData != null) {
				message.updateResponseInfo(responseData.clientData,responseData.clientCallback);
				this.handleResponseInternal(message, responseData.context, responseData.responseHandler);
			}
			else {
				this.logger.error(message.getRootName()+" responseData is null ignoring response");
			}
			return;	
		}
		else if(Message.REQUEST.equals(message.getType())) {
			this.handleRequestInternal(message);
			if(message.getState() != Message.REQUEST_WAITING_FOR_ASYN_RESPONSE) {
				Message response = Message.createResponse(message.getFrom(), message);
				response.setMessageBusClassName(message.getMessageBusClassName());
				this.sendMessage(response);
			}
			return;
		}
		else if(Message.EVENT.equals(message.getType()))
			this.handleEventInternal(message);
	}

	@Override
	public DeviceAdminAgentI getDeviceAdminAgentImpl() {
		return deviceAdminAgent;
	}

	@Override
	public LayoutAgentI getLayoutAgentImpl() {
		return layoutAgent;
	}

	@Override
	public MediaAgentI getMediaAgentImpl() {
		return mediaAgent;
	}

	@Override
	public MediaStoreAgentI getMediaStoreAgentImpl() {
		return mediaStoreAgent;
	}

	public void sendMessage(Message m) throws Exception {
		String  messageBusClassName = m.getMessageBusClassName();
		if(messageBusClassName!=null && messageBusClassName.equals(HTTPMessageBus.class.getName())) {
			this.logger.info("DeviceImpl: sendMessage: on= " + messageBusClassName + " name= " + m.getRootName() + " from= " + m.getFrom() + " to= " + m.getTo() + " service= " + m.getService());
			this.http.sendMessage(m);
		} else {
			this.logger.info("DeviceImpl: sendMessage: on= " + XMPPMessageBus.class.getName() + " name= " + m.getRootName() + " from= " + m.getFrom() + " to= " + m.getTo() + " service= " + m.getService());
			if(m.getFrom().equals(m.getTo())) {
				this.handleMessage(m);
			} else {
				this.xmpp.sendMessage(m);
			}
		}
	}

	@Override
	public void login(String ip, String user, String password, String resource, String appServerJID) throws Exception {
		if(loginInfo == null) {
			loginInfo = new LoginInfo(this,logger);
		}
		loginInfo.setLoginInfo(ip, user, password, resource, appServerJID);
		loginDaemon.startLoginDaemon(loginInfo);
	}	

	@Override
	public void logout() {
		try {
			//TODO: cleanup
			resetDeviceDB();
			resetAgents();
			this.xmpp.logout();
			if(loginInfo != null) {
				// Change the login state
				loginInfo.setLoginState(LoginInfo.LoginStateEnum.NotLoggedIn.toString());
			}
		}
		catch(Exception e){
			this.logger.debug("BaseDeviceImpl:logout error=" + e.toString(), e);
		}
		this.loginDaemon.stopLoginDaemon();
	}
	
	@Override
	public void sendRequest(Message message, Document context, ResponseHandlerI responseHandler)
			throws Exception {
		
		if(message.getTo() == null) {
			throw new Exception("Empty JID not allowed");
		}
		
		if(message.getFrom().equals(message.getTo())) {
			this.handleRequestInternal(message);
			Message response = Message.createResponse(message.getFrom(), message);
			this.handleResponseInternal(response, context, responseHandler);
		}
		else {
			if(Message.REQUEST.equals(message.getType())) {
				String clientData = ResponseData.generateClientData(
						message.getClientData(), context);
				String callbackData = responseHandler.getClass().getName();
				message.updateResponseInfo(clientData,callbackData);
			}
			this.logger.info("DeviceImpl: sendRequest " + message.getRootName() + " type= " + message.getType() + " from= " + message.getFrom() + " to= " + message.getTo() + " service= " + message.getService());
			this.xmpp.sendMessage(message);
		}
	}

	
	/*@Override
	public StreamCache getStreamCache() {
		return this.streamCache;
	}*/

	@Override
	public String getServiceVersion() {
		String SERVICE_VERSION_FILE_NAME = "service.version";
		String SERVICE_VERSION_FILE_PATH = "/com/ipvs/xsd/objectXml/" + SERVICE_VERSION_FILE_NAME;
		if(serviceVersion == null) {
			try {
				Document serviceVersionDoc = XMLUtils.loadXMLResource(SERVICE_VERSION_FILE_PATH);    	
				if(serviceVersionDoc == null) {
					serviceVersion = "0";
				}
				String schemaVersionStr  = XMLUtils.getAttributeValueWithDefault(serviceVersionDoc.getRootElement(), "Ver","0");    	  
				serviceVersion =  schemaVersionStr;
				logger.debug("Current Service Version = "+serviceVersion);		
			} catch(Exception exp) {
				exp.printStackTrace();
			}
		}
		return serviceVersion;
	}
	
	@Override
	public DeviceDBI getDeviceDB() {
		return deviceDB;
	}

		
	/////////////////// Logger Methods ////////////////////
	
	private void initLogger() {
		this.logger = Logger.getLogger(getLoggerCategory());
		Log4JErrorHandler errHandler = new Log4JErrorHandler();
		
		if(this.logger != null) {
			Enumeration<?> appenders = this.logger.getAllAppenders();
			if(appenders != null) {
				while(appenders.hasMoreElements()) {
					Appender append = (Appender)appenders.nextElement();
					append.setErrorHandler(errHandler);
				}
			}
		}
	}
	
	public String getLoggerCategory() {
		return LOGGER_CATEGORY;
	}
	
	public void setLoggerCategory(String category) {
		LOGGER_CATEGORY = category;
		initLogger();
	}
	
	@Override
	public Logger getLogger() {
		if(this.logger == null)
			initLogger();
		return this.logger;
	}
	
	@Override
	public void handleLog(String logStr, Level level, Throwable ex) {
		addLogWithLevel(logStr, level, ex);		
	}
	
	@Override
	public void messageTrace(String logStr, Message message, Throwable ex) {
//		if(getLogger().getLevel().isGreaterOrEqual(message.getLogLevel())) {
//			addLogWithLevel(logStr, getLogger().getLevel(), ex);
//		} else {
//			addLogWithLevel(logStr, message.getLogLevel(), ex);
//		}
		
	}
	
	private void addLogWithLevel(String logStr, Level level, Throwable ex) {
		if(Level.TRACE.equals(level))
			this.logger.trace(logStr, ex);
		else if(Level.DEBUG.equals(level))
			this.logger.debug(logStr, ex);
		else if(Level.INFO.equals(level))
			this.logger.info(logStr, ex);
		else if(Level.WARN.equals(level))
			this.logger.warn(logStr, ex);
		else
			this.logger.error(logStr, ex);
	}
	
/*
 * resets the DeviceDB entrees on logout
 */
	private void resetDeviceDB(){		
		deviceDB.setDeviceResourceNID("");
		deviceDB.setDeviceXMLNID("");
		deviceDB.resetPortResourceTable();
		
	}
	
	public String getAppserverJID() {
		if(xmpp==null) 
			return "";
		return xmpp.getAppServerAgentJID();
	}
	
	public String getMyJID() {
		// TODO Auto-generated method stub
		if(xmpp == null) 
			return "";
		return xmpp.getMyJID();
	}

	public LoginInfo getLoginInfo() {
		return loginInfo;
	}

	@Override
	public void queueNotification(String key, Message m) {
		this.notifier.queueNotification(key, m);
	}

	@Override
	public void sendNotification(String key, Message m) {
		this.notifier.sendNotification(key, m);
	}

	@Override
	public void handleLoginInfoChanged(String lastEvent) {
		//if the last event is not shutdown event reinitialize agents. 
		switch(LoginInfoEvents.valueOf(lastEvent)) {
		case Shutdown:
			//if last event is shutdown we don't reset current state.
			return;
			//for all other cases we will re intialize agents.
		case ConfigChanged:
		case DiscoveredNewSC:
			logger.info("DeviceImpl: handleLoginInfoChanged: Reinitialize Agents");
			initAgents();
			deviceDB.getStreamCache().resetCache();
		default:
			return;
		}
	}

}
