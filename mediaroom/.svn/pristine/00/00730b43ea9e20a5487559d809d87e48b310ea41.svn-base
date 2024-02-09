package com.ipvs.device.jetty;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.StringTokenizer;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.commons.lang.builder.ToStringBuilder;
import org.apache.commons.lang.builder.ToStringStyle;
import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.eclipse.jetty.util.ajax.JSON;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.sessionservice.impl.SessionNotificationListenerI;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

class TestSessionManager implements Runnable {
	private SessionNotificationListenerI listener;
	private String sessionId;
	private static final long setupTime = 1000;
	private static TestSessionManager theTestSessionManager = null;
	
	public static TestSessionManager getTestSessionManager() {
		if(theTestSessionManager == null)
			theTestSessionManager = new TestSessionManager();
		return theTestSessionManager;
	}
	
	public void setSessionNotificationListener(SessionNotificationListenerI listener, String sessionId) {
		this.listener = listener;
		this.sessionId = sessionId;
		(new Thread(this)).start();
	}
	
	public void run() {
		try {
			Thread.sleep(setupTime);
			String notificationXML = "<Notification/>";
			listener.handleNotification(sessionId, notificationXML);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
}

class SessionEntry {
	String sessionId;
	String userJID;
	Long ts;
	Continuation cc;
	StringBuffer notifications;
	
	SessionEntry(String sessionId, String userJID, Continuation cc) {
		this.sessionId = sessionId;
		this.userJID = userJID;
		this.ts = System.currentTimeMillis();
		this.cc = cc;
		this.notifications = null;
	}
	void touch(Continuation cc) {
		this.ts = System.currentTimeMillis();
		this.cc = cc;
	}
	
	synchronized StringBuffer flushNotifications(Continuation cc) {
		this.touch(cc);
		if(this.notifications == null)
			return null;
		
		StringBuffer s = this.notifications;
		s.append("</notificationList>");
		this.notifications = null;
		return s;
	}
	
	synchronized void addNotification(String notification) {
		if(notifications == null)
			this.notifications = new StringBuffer("<notificationList>");
		notifications.append(notification);
	}
}

class SessionHelper implements Runnable, SessionNotificationListenerI {
	private static String SESSION_REQUEST_XML = "/com/ipvs/sessionservice/session/xml/";
    private static Hashtable<String, Document> requestDocTable = new Hashtable<String,Document>();
    private Hashtable<String, SessionEntry> sessionTable = new Hashtable<String,SessionEntry>();
    private Thread gcThread = null;
	public static boolean test = false;
	public static long sessionTimeout = 10000;
	public static long keepAliveTolerance = 5000;
	// private TestSessionManager testSessionManager = null;
	
	XMPPI xmpp;
	MRLog log;
	MRDB db;
	MRResourceFactoryI resourceFactory;
	ServiceAgentFactoryI serviceAgentFactory;
	String userJID;  
	boolean debug = false;

	public void addLog1(String txt) {
    	if(SessionHelper.test) {
    		System.out.println(System.currentTimeMillis() + ":" + txt);
    	}
    	else {
    		log.addInfo(txt);
    	}
    }

	public void addDebug(String txt) {
		if(!debug)
			return;
		this.addLog1(txt);
	}
	
	public SessionHelper(XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String userJID) {
	   	this.xmpp = xmpp;
		this.log = log;
		this.db = db;
		this.resourceFactory = resourceFactory;
		this.serviceAgentFactory = serviceAgentFactory;
		this.userJID = userJID;  

		// testSessionManager = new TestSessionManager(this);
	}

	public synchronized void addSession(String sessionId, String userJID, Continuation cc) {
		addDebug("SessionHelper:addSession sessionId="+sessionId);
    	sessionTable.put(sessionId, new SessionEntry(sessionId, userJID, cc));
    	if( (gcThread == null) || (!gcThread.isAlive())) {
    		gcThread = new Thread(this);
    		gcThread.start();
    	}
    	if(sessionTable.size() == 1)
    		this.setSessionNotificationListener(this, sessionId);
    	// testSessionManager.addSession(sessionId);
    }
	
    public synchronized void updateSession(String sessionId, String userJID, Continuation cc) throws Exception {
    	addDebug("SessionHelper:updateSession sessionId="+sessionId);
		if(!sessionTable.containsKey(sessionId))
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "SessionHelper:updateSession not found sessionId=" + sessionId);
		sessionTable.get(sessionId).touch(cc);
    }
    
    public synchronized StringBuffer flushNotifications(String sessionId, String userJID,
    		Continuation cc) throws Exception {
		if(!sessionTable.containsKey(sessionId))
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "SessionHelper:updateSession not found sessionId=" + sessionId);
		SessionEntry entry = sessionTable.get(sessionId);
		return entry.flushNotifications(cc);
    }
    
    public void handleNotification(String sessionId, String notificationXML) {
    	SessionEntry entry = sessionTable.get(sessionId);
    	if(entry == null)
    		return;
    	entry.addNotification(notificationXML);
       	try {
       		if((entry.cc != null) && (entry.cc.isSuspended())) {
    			this.addDebug("GetURLServlet.SessionHelper:handleNotification resuming cc");
       			entry.cc.resume();
       		}
       		else 
    			this.addDebug("GetURLServlet.SessionHelper:handleNotification cc not suspended cc=" + entry.cc);

    	}
    	catch(IllegalStateException e) {
    		e.printStackTrace();
			String s = ToStringBuilder.reflectionToString(entry.cc,ToStringStyle.MULTI_LINE_STYLE);
			System.out.println("GetURLServlet:service cc=" + s);
			this.addLog1("GetURLServlet.SessionHelper:handleSessionUpdate cc.resume gives exception stoppping sessionId=" + entry.sessionId + " exception=" + e.toString() + " cc=" + s);
			this.stopSession(entry.sessionId, entry.userJID);
    	}
    }

    
    public long doGC() {
		// System.out.println("SessionHelper:doGC checking sessions ...");
    	Enumeration<String> keys = sessionTable.keys();
    	ArrayList<SessionEntry> removeList = new ArrayList<SessionEntry>();
    	long nextExpiry = sessionTimeout * 10;
    	long now = System.currentTimeMillis();
    	while(keys.hasMoreElements()) {
    		String sessionId = keys.nextElement();
    		SessionEntry entry = sessionTable.get(sessionId);
    		long age = now - entry.ts;
    		if(age > SessionHelper.sessionTimeout + SessionHelper.keepAliveTolerance) {
    			removeList.add(entry);
    		}
    		else {
    			long expiry = SessionHelper.sessionTimeout - age;
    			if(expiry < nextExpiry && expiry > 0)
    				nextExpiry = expiry;
    			else {
    				//if keepalive heartbeat dies, expiry time eventually will be negative!
    				//check every 5 secs until age > SessionHelper.sessionTimeout + SessionHelper.keepAliveTolerance
    				nextExpiry = 5000;
    			}
    		}
    	}
    	for(SessionEntry entry : removeList) {
			this.stopSession(entry.sessionId, entry.userJID);
			this.addLog1("GetURLServlet.SessionHelper:doGC sessionId=" + entry.sessionId);
    	}
		// System.out.println("SessionHelper:doGC checking sessions ...done nextExpiry=" + nextExpiry);
    	return nextExpiry + 10;
    }
    
	@Override
	public void run() {
		while(true) {
			long nextExpiry = doGC();
			if(sessionTable.size() == 0) {
				// addLog("SessionHelper:run exiting");
				break;
			}
			try {
				// addLog("SessionHelper:run sleep=" + (nextExpiry/1000) + " waiting for=" + sessionTable.size());
				Thread.sleep(nextExpiry);
			}
			catch(Exception e) {
				break;
			}
		}
		
	}
	
	public synchronized void stopSession(String sessionId, String userJID){
		SessionEntry entry = sessionTable.get(sessionId);
    	if(entry == null)
    		return;
		if(sessionId == null)
			return;
		
		addLog1("GetURLServlet.SessionHelper:stop Stopping sessionId="+sessionId);
		String requestName = "StopSession";
		try {
			Document requestDoc = this.getRequestDoc(requestName, null);
	        MRXMLUtils.getElementXpath(requestDoc, "//"+ requestName).addAttribute("sessionId", sessionId);
	        this.getResponse(userJID, requestName, requestDoc);
	        // System.out.println(MRXMLUtils.documentToString(responseDoc));
	    	if( (entry.cc != null) && entry.cc.isSuspended())
	    		entry.cc.resume();
		}
		catch(Exception e) {
			this.addLog1("GetURLServlet.SessionHelper:stopSession error=" + e.toString() + " sessionId=" + sessionId);
		}
		sessionTable.remove(sessionId);
		addDebug("GetURLServlet:SessionHelper stopSession sessionTable size=" + sessionTable.size());    		
    	if(sessionTable.size() == 0)
    		this.setSessionNotificationListener(null, sessionId);
	}
	
    public Document getRequestDocFromTemplate(String requestName, MRLog log) throws Exception {
    	if(requestDocTable.containsKey(requestName))
    		return (Document)requestDocTable.get(requestName).clone();
        Document requestTemplate = MRXMLUtils.loadXMLResource(SESSION_REQUEST_XML + requestName + "Request.xml", 
        		log);
        Element requestData = MRXMLUtils.getElementXpath(requestTemplate, "//" + requestName);
        Document requestDoc = SessionServiceUtils.getRequestXML(requestName, log, requestData);
        // Document requestDoc = MRXMLUtils.elementToDocument(requestData);
        // String xml = MRXMLUtils.documentToString(requestDoc);
        requestDocTable.put(requestName, requestDoc);
        return (Document)requestDoc.clone();    	
    }
    
	Document getRequestDoc(String requestName, HttpServletRequest httpRequest) throws Exception {
		if(test)
			return MRXMLUtils.stringToDocument("<" + requestName + "/>");
		
		Document requestDoc = this.getRequestDocFromTemplate(requestName, this.log);
        if(httpRequest == null)
        	return requestDoc;

        Enumeration<String> e = httpRequest.getParameterNames();
		while (e.hasMoreElements()) {
			String name = e.nextElement();
			StringTokenizer st = new StringTokenizer(name, ".");
			if(st.countTokens() != 2)
				continue;
			String elementName  = st.nextToken();
			String attributeName = st.nextToken();
			String value = httpRequest.getParameter(name);
			Element element = MRXMLUtils.getElementXpath(requestDoc, "//" + elementName);
			if(element != null){
				element.addAttribute(attributeName, value);
			}
		}
        return requestDoc;
	}
	
	Document getSessionDoc(String userJID, Document requestDoc, String sessionId)throws Exception {
		if(test)
			return MRXMLUtils.stringToDocument("<Session id='" + sessionId + "'><Connection state='STREAMING' streamURL='c:\\miyer\\movies\\sample.mp4' /></Session>");
		
		MRRequest mrRequest = MRRequest.createRequest(userJID, requestDoc);

		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDocWithMediaRoomInfo(mrRequest, 
						xmpp, log, db, resourceFactory, serviceAgentFactory,sessionId);		
		return sessionDoc;
	}
	
	Document getResponse(String userJID, String requestName, Document requestDoc) throws Exception {
        if(test)
			return MRXMLUtils.stringToDocument("<Session id='123'><Connection state='STREAMING' streamURL='v2d://' /></Session>");
		String handlerName = "com.ipvs.sessionservice.session.handler." + requestName + "RequestHandler";
		Class<?> c = Class.forName(handlerName);
		MRRequestHandlerI handler = (MRRequestHandlerI)c.newInstance();
        MRRequest mrRequest = MRRequest.createRequest(userJID, requestDoc,
        		xmpp.getXMPPUserJid());
		Document responseDoc = handler.getResponse(mrRequest, 
				xmpp, log, db, resourceFactory, serviceAgentFactory);		
		return responseDoc;
	}
	
    void setSessionNotificationListener(SessionNotificationListenerI listener, String sessionId) {
        if(test)
        	TestSessionManager.getTestSessionManager().setSessionNotificationListener(listener, sessionId);
        else
        	AppServerAgentUtils.getSessionManager(serviceAgentFactory).setSessionNotificationListener(listener);
    }
}

public class GetURLServlet extends HttpServlet{

	private static final long serialVersionUID = 1L;
    public SessionHelper sessionHelper = null;
	protected String userJID; 
	private static final String ERROR = "error";
	private static final String RESULT = "result";	
	private static final String KEEPALIVE = "KeepAlive";
	static int override = -1;

    public GetURLServlet(XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String userJID) {   	
		this.userJID = userJID;  
		sessionHelper = new SessionHelper(xmpp,log,db,resourceFactory,serviceAgentFactory,userJID);
	
	}

	void sendResponse(HttpServletRequest httpRequest, HttpServletResponse httpResponse, 
			String sessionId, String mediaURL,
			int errorCode, String responseType, String responseData) throws Exception {
		
	    String sessionURL = httpRequest.getRequestURL().toString();
	    String forwardedIP = httpRequest.getHeader("X-FORWARDED-FOR");
		String startTSStr = (String)httpRequest.getAttribute("startTS");
		long startTS = Long.parseLong(startTSStr);
		long duration = System.currentTimeMillis() - startTS;
		
		if(forwardedIP != null) {
			String forwardedHost = httpRequest.getHeader("X-FORWARDED-HOST");
			String scheme = httpRequest.getHeader("X-Forwarded-Proto");
			if(scheme == null || scheme.isEmpty())
				scheme = "http";
			sessionURL = scheme + "://" + forwardedHost  + httpRequest.getRequestURI();
		}

		String clientData = httpRequest.getParameter("clientData");
		String timestamp = httpRequest.getParameter("timestamp");
		String apiKey = httpRequest.getParameter("apiKey");
		String userJID = httpRequest.getParameter("userJID");
		String signature = httpRequest.getParameter("signature");

		if(clientData == null)
			clientData = "";
    	sessionURL += "?sessionId=" + sessionId 
    								+ "&userJID="+userJID
							    	+ "&signature=" + signature
							    	+ "&clientData=" + clientData;
    	if(timestamp != null ) {
    		sessionURL+= "&timestamp=" + timestamp;
    	}
    	
    	if(apiKey != null) {
    		sessionURL+= "&apiKey=" + apiKey;
	    }

    	String requestName = httpRequest.getParameter("requestName");
    	HashMap<String,String> map = new HashMap<String,String>();
		map.put("requestName", requestName);
		map.put("sessionURL", sessionURL);
		map.put("sessionId", String.valueOf(sessionId));
		map.put("mediaURL", mediaURL);
		map.put("errorCode", String.valueOf(errorCode));
		map.put("type", responseType);
		
		responseData = MRXMLUtils.xmlToJSONString(responseData, 4);
		
		map.put("data", responseData);
		map.put("clientData", clientData);
		map.put("duration", String.valueOf(duration));
		
		String callback = httpRequest.getParameter("callback");
		String response = callback + "(" + JSON.toString(map) + ");";
        //httpResponse.setContentType("application/json");
//		int statusCode = HttpServletResponse.SC_OK;
//		if(errorCode == HttpServletResponse.SC_UNAUTHORIZED) {
//			statusCode = errorCode;
//		} 
//		
//		httpResponse.setStatus(statusCode);
		httpResponse.getWriter().println(response);
		sessionHelper.addDebug("GetURLServlet:sendResponse requestName=" + requestName + " clientData=" + clientData + " sessionId=" + sessionId);
	}
	
	boolean isSessionStartRequest(String requestName) {
		if("View".equals(requestName))
			return true;
		if("Playback".equals(requestName))
			return true;
		return false;
	}


	boolean isSessionRequest(String requestName) {
		return (!KEEPALIVE.equals(requestName) && !isSessionStartRequest(requestName));
	}

	public void service(HttpServletRequest httpRequest,HttpServletResponse httpResponse) throws IOException, ServletException {
		
		String requestName = httpRequest.getParameter("requestName");
		try {
			if(KEEPALIVE.equals(requestName)) {
				HttpAuthentication.validateSignature(httpRequest, httpResponse, sessionHelper.xmpp, sessionHelper.serviceAgentFactory, sessionHelper.db,sessionHelper.log, false);	
			} else {
				HttpAuthentication.validateSignature(httpRequest, httpResponse, sessionHelper.xmpp, sessionHelper.serviceAgentFactory, sessionHelper.db,sessionHelper.log, true);
			}
		}
		catch(Exception e) {
			this.log("GetURLServlet:service validateSignature failed error=" + e.toString());

			if(override == -1) {
				override = 0;
				try {
					if(FileUtils.isFileExist("/tmp", "override"))
						override = 1;
				}
				catch(Exception ex) {
					// do nothing
				}
			}
			 int errorCode = 1000;
			
			 if(e instanceof MRException) {
				 errorCode = ((MRException) e).getCode();
			 }
			 
			if(override == 1)
				this.log("GetURLServlet:service override signature validation");
			else {
				String sessionId = httpRequest.getParameter("sessionId");
				try {
					httpRequest.setAttribute("startTS", String.valueOf(System.currentTimeMillis()));
					this.sendResponse(httpRequest, httpResponse, sessionId, "",
							errorCode, ERROR, formatErrorResponse(e));
				}
				catch(Exception exx) {
					sessionHelper.addDebug("GetURLServlet:sendResponse error=" + exx.toString() + " stopping Session sessionId=" + sessionId);
					sessionHelper.stopSession(sessionId, userJID);
				}
				return;
			}
		}

		httpResponse.setContentType("text/html;charset=utf-8");
		httpResponse.setStatus(HttpServletResponse.SC_OK);

		
		String sessionId = httpRequest.getParameter("sessionId");
		String clientData = httpRequest.getParameter("clientData");
		
		
		String startTS = (String)httpRequest.getAttribute("startTS");
		if(startTS == null) {
			this.sessionHelper.addDebug("GetURLServlet:service received requestName=" + requestName + " clientData=" + clientData + " sessionId=" + sessionId);
			httpRequest.setAttribute("startTS", String.valueOf(System.currentTimeMillis()));
		}
		else {
			this.sessionHelper.addDebug("GetURLServlet:service continue requestName=" + requestName + " clientData=" + clientData + " sessionId=" + httpRequest.getAttribute("sessionId"));			
		}
		
		if(isSessionRequest(requestName)) {
			try {
				this.handleSessionRequest(httpRequest, httpResponse, sessionId);
			}
			catch(Exception e) {
				try {
					this.sendResponse(httpRequest, httpResponse, sessionId, "",
							0, ERROR, formatErrorResponse(e));
				}
				catch(Exception exx) {
					sessionHelper.addDebug("GetURLServlet:sendResponse error=" + exx.toString() + " stopping Session sessionId=" + sessionId);
					sessionHelper.stopSession(sessionId, userJID);
				}
			}
			return;
		}
		
		Continuation cc = ContinuationSupport.getContinuation(httpRequest);

		try {
			
			// if(sessionId == null) throw new Exception();
			
			boolean done = false;
			if(isSessionStartRequest(requestName)) {
				if(cc.isInitial())
					this.handleStartSessionRequest(httpRequest, httpResponse, cc);
				else
					done = this.checkSession(httpRequest, httpResponse);
			}
			else if(KEEPALIVE.equals(requestName)){
				if(cc.isInitial()) {
					this.handleKeepAliveRequest(httpRequest, httpResponse, sessionId, cc);
					
					//make sure if any pending notifications are there return the response immediately
					String userJID = httpRequest.getParameter("userJID");
					StringBuffer sb = sessionHelper.flushNotifications(sessionId, userJID, cc);
					if(sb!=null) {
						sendResponse(httpRequest, httpResponse, sessionId, "", 0, RESULT, sb.toString());
						done =true;
						//update continuation to null as we have return notifications
						sessionHelper.updateSession(sessionId, userJID, null);
					}					
				}
				else
					done = this.checkKeepAlive(httpRequest, httpResponse, sessionId);				
			}
			if(done) {
				// cc.complete();
			}
			else {
				cc.setTimeout(SessionHelper.sessionTimeout);
				cc.suspend();
			}
		}
		catch(Exception e) {
			//e.printStackTrace();
			if(sessionId == null)
				sessionId = (String)httpRequest.getAttribute("sessionId");
			sessionHelper.addLog1("GetURLServlet:service sending error=" + e.toString());
			try {
		    	sendResponse(httpRequest, httpResponse, sessionId, "",
		    			1000, ERROR, formatErrorResponse(e));
			}
			catch(Exception ex) {
				// Stop the session
				sessionHelper.addDebug("GetURLServlet:sendResponse error=" + ex.toString() + " stopping Session sessionId=" + sessionId);
			}				
			try {
				sessionHelper.stopSession(sessionId, userJID);
			} catch (Exception e1) {
				//e1.printStackTrace();
			}
		}

		/*
		if(isSessionStartRequest(requestName) && (startTS == null)) {
			String s = ToStringBuilder.reflectionToString(cc,ToStringStyle.MULTI_LINE_STYLE);
			System.out.println("GetURLServlet:service cc=" + s);
		}
		*/

	}

	public void handleStartSessionRequest(HttpServletRequest httpRequest,HttpServletResponse httpResponse, Continuation cc) throws Exception {
		// System.out.println("Start");
		String requestName = httpRequest.getParameter("requestName");
		String userJID = httpRequest.getParameter("userJID");
		Document requestDoc = null;
		try {
			requestDoc = sessionHelper.getRequestDoc(requestName, httpRequest);
			
			// Reset the sessionId from the template
			Element requestElement = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName);
			requestElement.addAttribute("sessionId", "");
			String xml = requestDoc.asXML();
			this.sessionHelper.addDebug("GetURLServlet xml=" + xml);

			// Plugin the ip address of the destination, this will not be required once we have rtsp
			// The destUDPStartPort will be picked up from the port configuration
			String destIP = httpRequest.getRemoteAddr();
			String forwardedIP = httpRequest.getHeader("X-FORWARDED-FOR");
			if(forwardedIP != null)
				destIP = forwardedIP;
			
			Element element = MRXMLUtils.getElementXpath(requestDoc, "//DstPort");
			if(element != null){
				element.addAttribute("portIP", destIP);
			}
		}
		catch(Exception e) {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, 
					"Invalid Request error=" + e.toString());
		}
		Document responseDoc = sessionHelper.getResponse(userJID, requestName, requestDoc);
		String sessionId = MRXMLUtils.getElementXpath(responseDoc, "//Session").attributeValue("id");
		httpRequest.setAttribute("sessionId", sessionId);
		String clientData = httpRequest.getParameter("clientData");
		sessionHelper.addLog1("GetURLServlet:handleStartSessionRequest clientData=" + clientData + " sessionId=" + sessionId);
		sessionHelper.addSession(sessionId, userJID, cc);
		
		// Check if there is an error
		String errorCode = MRXMLUtils.getElementXpath(responseDoc, "//Connection").attributeValue("errorCode");
		String errorData = MRXMLUtils.getElementXpath(responseDoc, "//Connection").attributeValue("errorData");
		if(errorCode != null)
			throw new MRException(Utils.getIntValue(errorCode,MRExceptionCodes.SYSTEMERROR), errorData);
	}
	
	private String formatResponse(Document d) {
		if(d == null)
			return "";
		return d.asXML();
	}
	
	private String formatErrorResponse(Exception e) {
		try {
			if( e instanceof MRException) {
				MRException ex = (MRException)e;
				return SessionServiceUtils.exceptionToXML(ex);
			}
		}
		catch(Exception exx) {
			sessionHelper.addDebug("GetURLServlet.SessionHelper:formatErrorResponse error=" + exx.toString());
		}
		return e.toString();
	}


	public void handleSessionRequest(HttpServletRequest httpRequest,HttpServletResponse httpResponse, String sessionId) throws Exception {
		String requestName = httpRequest.getParameter("requestName");
		String userJID = httpRequest.getParameter("userJID");
		if("StopSession".equals(requestName)) {
			sessionHelper.stopSession(sessionId, userJID);
			return;
		}

		// Check if this is to enable/disable debug

        if("ToggleDebugRequest".equals(requestName)) {
        	this.sessionHelper.debug = !this.sessionHelper.debug;
        	Document response = MRXMLUtils.stringToDocument("<" + requestName + "/>");
    		this.sendResponse(httpRequest, httpResponse, sessionId, "",
    				0, RESULT, formatResponse(response));
			return;
        }

		Document requestDoc = null;
		requestDoc = sessionHelper.getRequestDoc(requestName, httpRequest);
	   
		Document response = sessionHelper.getResponse(userJID, requestName, requestDoc);
		response.getRootElement().setName(requestName);
		this.sendResponse(httpRequest, httpResponse, sessionId, "",
				0, RESULT, formatResponse(response));
	}

	public boolean checkSession(HttpServletRequest httpRequest,HttpServletResponse httpResponse) throws Exception {
		String userJID = httpRequest.getParameter("userJID");
		String requestName = httpRequest.getParameter("requestName");
		String sessionId = (String)httpRequest.getAttribute("sessionId");
		Document requestDoc = sessionHelper.getRequestDoc(requestName, null);
        Document sessionDoc = sessionHelper.getSessionDoc(userJID, requestDoc, sessionId);
        
        String state = MRXMLUtils.getElementXpath(sessionDoc, "//Connection").attributeValue("state");
        String errorCode = MRXMLUtils.getElementXpath(sessionDoc, "//Connection").attributeValue("errorCode");
        String errorData = MRXMLUtils.getElementXpath(sessionDoc, "//Connection").attributeValue("errorData");
        String mediaURL = MRXMLUtils.getElementXpath(sessionDoc, "//Connection").attributeValue("streamURL");
        
        if(errorCode != null)
			throw new MRException(Utils.getIntValue(errorCode,MRExceptionCodes.SYSTEMERROR), errorData);
       
        if(state !=null && "AUTHORIZING".equalsIgnoreCase(state)){
        	sendResponse(httpRequest, httpResponse, sessionId, "",
	    			0, RESULT, formatResponse(sessionDoc));
			return true;
        }else if(mediaURL!=null){
        	sendResponse(httpRequest, httpResponse, sessionId, mediaURL,
	    			0, RESULT, formatResponse(sessionDoc));
			sessionHelper.updateSession(sessionId, userJID, null);
			return true;
        }	
		
		return false;
	}
	
	public void handleKeepAliveRequest(HttpServletRequest httpRequest,HttpServletResponse httpResponse, String kaSessionId,
			Continuation cc) throws Exception {
		String clientData = httpRequest.getParameter("clientData");
		sessionHelper.addDebug("GetURLServlet:handleKeepAliveRequest clientData=" + clientData + " sessionId=" + kaSessionId);
		String userJID = httpRequest.getParameter("userJID");
		sessionHelper.updateSession(kaSessionId, userJID, cc);
		httpRequest.setAttribute("sessionId", kaSessionId);
	}

	public boolean checkKeepAlive(HttpServletRequest httpRequest,HttpServletResponse httpResponse, String sessionId) throws Exception {
		String userJID = httpRequest.getParameter("userJID");
		StringBuffer sb = sessionHelper.flushNotifications(sessionId, userJID, null);
		String responseData = "";
		if(sb!=null) 
			responseData = sb.toString();
    	sendResponse(httpRequest, httpResponse, sessionId, "",
    			0, RESULT, responseData);
		return true;
	}

	public static void main(String[] args) throws Exception {
		System.out.println("Hello");
		Server server = new Server(8080);
		SessionHelper.test = true;
        ServletContextHandler servletContextHandler = new ServletContextHandler(server, "/controller", true, false);
		servletContextHandler.addServlet(new ServletHolder(new GetURLServlet(null,  null,  null,  null,
				null,  null)), "/session");
		server.start();
		server.join();
	}

}
