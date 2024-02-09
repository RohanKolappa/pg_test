package com.ipvs.sessionservice.service.handler;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;

import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Request;
import org.jboss.resteasy.util.HttpResponseCodes;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpConst;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.http.servlet.IPVSHttpServlet;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public class NotificationChannelServlet extends IPVSHttpServlet {

	private static final long serialVersionUID = 1L;
	private static final boolean debug = false; 

	public NotificationChannelServlet(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, String userJID) {
		super(xf, log, db, resourceFactory, userJID);

	}

	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {
		// TODO Auto-generated method stub

	}

	public void addLog(String message, int code, int level) {
		if(debug)
			log.addInfo(message, code, level);
	}

	protected void doGet(HttpServletRequest req, HttpServletResponse resp)	throws ServletException, IOException {
		String acceptHeader = req.getHeader(HttpHeaders.ACCEPT);
		if(acceptHeader == null || (!acceptHeader.equals(MediaType.APPLICATION_JSON) && !acceptHeader.equals(MediaType.APPLICATION_XML))) {
			// set to default json, to return error to client.
			acceptHeader = MediaType.APPLICATION_JSON;
			
			log.addInfo("NotificationChannelRequestHandler: Error. Accept Header Incorrect or Missing!", MRLog.INFO, MRLog.NOTIFY);	
			try {
				returnHttpResponse(req, resp, new MRException(HttpServletResponse.SC_NOT_ACCEPTABLE, "Error. Accept Header Incorrect or Missing!").toString(), HttpServletResponse.SC_NOT_ACCEPTABLE,acceptHeader,0,"",false);
			} catch (Exception e) {
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
				
			return;
		}
			
		String token = req.getParameter("token");
		if(token == null) {
				addLog("NotificationChannelRequestHandler: handleGetRequest token:="+token,MRLog.INFO,MRLog.NOTIFY);	
			try {
				returnHttpResponse(req, resp, new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid token").toString(), HttpServletResponse.SC_UNAUTHORIZED,acceptHeader,0,"",false);
			} catch (Exception e) {
				//ignore response if client goes away
				log.addInfo("Can not write response to  request uri =" + req.getRequestURI(),MRLog.INFO,MRLog.NOTIFY);
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
			return;
		}
		Continuation requestContinuation = ContinuationSupport.getContinuation(req);			

		AuthTokenCache authTokenCache=null;
		AuthTokenCache.UserResource userResourceFromCache=null;
		AuthTokenCache.UserResource parsedUserResource=null;

		String authHeader = req.getHeader(HttpHeaders.AUTHORIZATION);
		String resource = req.getHeader(HttpConst.X_BARCO_RESOURCE_HEADER);

		try {
			authTokenCache = AppServerAgentUtils.getAuthTokenCache(xf.sf(userJID));
			parsedUserResource = authTokenCache.parseAuthHeader(authHeader,resource);
		} catch (Exception e1) {
			//ignore exception
			log.addInfo("NotificationChannelRequestHandler: Exception occur while finding the auth cache:"+MRException.getStackTrace(e1),MRLog.INFO,MRLog.NOTIFY);
			try {
				returnHttpResponse(req, resp, new MRException(HttpServletResponse.SC_BAD_REQUEST, "Not able to parse auth header. Invalid Request").toString(), HttpServletResponse.SC_BAD_REQUEST,acceptHeader,0,"",false);
			} catch (Exception e) {
				//this should not happend if happend then return
				log.addInfo("Can not write response to  request uri =" + req.getRequestURI(),MRLog.INFO,MRLog.NOTIFY);
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
			return;
		}

		// Check if this is a kick out or usual logout
		if(requestContinuation != null && requestContinuation.getAttribute("kick") != null) {
			String kick = (String)requestContinuation.getAttribute("kick");
			String statusStr = (kick.equalsIgnoreCase("true")) ? "KickedOut" : "LoggedOut";
			//TBD we should return proper error code for kicked out 
			//int statuCode = (kick.equalsIgnoreCase("true")) ? HttpResponseCodes.SC_INTERNAL_SERVER_ERROR : HttpResponseCodes.SC_OK;
			addLog("NotificationChannelRequestHandler: User: " + statusStr + " : " + parsedUserResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
			String notification ="<NotificationList><"+statusStr+"/></NotificationList>";
			try {
				returnHttpResponse(req, resp,notification , HttpResponseCodes.SC_OK,acceptHeader,0,"",false);
			} catch (Exception e) {
				log.addInfo("Can not write response to  request uri =" + req.getRequestURI() +" username:="+parsedUserResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}

			addLog("End NotificationChannel For user =" + parsedUserResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
			return;


		}

		userResourceFromCache = authTokenCache.getUserResource(parsedUserResource.getUsername());

		//make sure user in the cache and token is matching 
		if(userResourceFromCache == null || !userResourceFromCache.getToken().equals(token)) {
			addLog("NotificationChannelRequestHandler: Unauthorized token:="+token,MRLog.INFO,MRLog.NOTIFY);
			try {
				returnHttpResponse(req, resp, new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid token").toString(), HttpServletResponse.SC_UNAUTHORIZED,acceptHeader,0,"",false);
			} catch (Exception e) {
				//ignore response if client goes away
				log.addInfo("Can not write response to  request uri =" + req.getRequestURI() +" username:="+parsedUserResource.getUsername() ,MRLog.INFO,MRLog.NOTIFY);
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
			return;
		}

		//long notificationTimestamp = Utils.getLongValue(req.getParameter("notificationtimestamp"),-1);

		//if client come back with latest url means user received the notifications then remove notifications from gc list.

		//		if(notificationTimestamp == userResourceFromCache.getNotificationTimestamp()) {
		//			userResourceFromCache.clearGCNotificationList();
		//		}

		//if notifications exists in the cache write results
		String notification  = userResourceFromCache.getNotifcations();
		if(notification != null) {

			addLog("Found Notification For user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);			
			try {

				returnHttpResponse(req, resp,notification, HttpServletResponse.SC_OK,acceptHeader,userResourceFromCache.getNotificationTimestamp(),userResourceFromCache.getToken(),true);
				userResourceFromCache.clearGCNotificationList();
			} catch (Exception e) {
				log.addInfo("Can not write notifications request uri =" + req.getRequestURI()+" user:="+userResourceFromCache.getUsername() ,MRLog.INFO,MRLog.NOTIFY);
				e.printStackTrace();
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}

			userResourceFromCache.setContinuation(null);
			addLog("End NotificationChannel For user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
			return;
		}	

		String expiryInterval = req.getParameter("expiryinterval");
		Continuation cachedContinuation =  null;
		cachedContinuation = userResourceFromCache.getContinuation();

		//this will handle the case where user started long poll and abort the request 
		if(cachedContinuation != null && !requestContinuation.equals(cachedContinuation)){
			addLog("Complete Existing Contiuation for user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
			try {
				cachedContinuation.complete();
			}
			catch(Exception e) {
				log.addInfo("Complete Existing Contiuation for user =" + userResourceFromCache.getUsername() + " error=" + e.toString(),MRLog.INFO,MRLog.NOTIFY);	
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
			cachedContinuation = null;
		}

		long expiry = Utils.getLongValue(expiryInterval, BaseHtppRequestHandler.CONTINUATION_EXPIRY_INTERVAL);

		//initialize continuation
		if(cachedContinuation == null) {				
			addLog("Initializing the NotificationChannel for user =" + userResourceFromCache.getUsername() +" URL:="+req.getRequestURL() +" queryString:="+req.getQueryString(),MRLog.INFO,MRLog.NOTIFY);
			cachedContinuation = requestContinuation;
			authTokenCache.addContinuationToUserResource(userResourceFromCache.getUsername(), requestContinuation);
			userResourceFromCache.setContinuationExpiryTime((System.currentTimeMillis() + expiry)/1000l);
		} 

		//check continuation expired
		if((System.currentTimeMillis()/1000l) >= userResourceFromCache.getContinuationExpiryTime()) {
			String emptyNotification = "<NotificationList/>";			
			try {
				returnHttpResponse(req, resp,emptyNotification, HttpServletResponse.SC_OK,acceptHeader,0,userResourceFromCache.getToken(),true);
			} catch (Exception e) {
				//ignore response if client goes away
				log.addInfo("Can not write response to  user =" + userResourceFromCache.getUsername() +" response:"+emptyNotification,MRLog.INFO,MRLog.NOTIFY);
				resp.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Error!" + e.getMessage());
			}
			userResourceFromCache.setContinuation(null);
			addLog("End NotificationChannel For user =" + userResourceFromCache.getUsername() +"URL:="+req.getRequestURL() +" queryString:="+req.getQueryString(),MRLog.INFO,MRLog.NOTIFY);
			return;
		}

		//suspend continuation
		cachedContinuation.setTimeout(expiry);
		cachedContinuation.suspend();
		addLog("suspend continuation for user =" + userResourceFromCache.getUsername() +" now:="+(System.currentTimeMillis()/1000l)  +" expiry:= "+userResourceFromCache.getContinuationExpiryTime(),MRLog.INFO,MRLog.NOTIFY);

	}

	private void returnHttpResponse(HttpServletRequest req, HttpServletResponse resp, String response,int statusCode,String acceptHeader, long notificationTimestamp, String token,boolean includeNewURL) throws Exception {

		//client has to come back with new url to make sure he received the notifications
		if(includeNewURL) {
			
			String baseURL = req.getRequestURL().toString();
			AppServerAgent agent = AppServerAgentUtils.getAppServerAgent(xf.sf(userJID));
			Element ApiKeyTableEl = agent.getAPIKey();
			String msIPAddress = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.MS_IPADDRESS, "");
			String forwardedHost =req.getHeader("X-Forwarded-Host");
			String scheme = req.getHeader("X-Forwarded-Proto");
			addLog("HTTPLoginRequest: scheme:="+scheme,MRLog.INFO,MRLog.NOTIFY);
			if(scheme == null || scheme.isEmpty())
				scheme = "http";
			if(forwardedHost != null) {
				addLog("HTTPLoginRequest: forwardedHost:="+forwardedHost,MRLog.INFO,MRLog.NOTIFY);
				msIPAddress = forwardedHost;
			}
			if(msIPAddress != null){
			baseURL = scheme + "://" + msIPAddress  + req.getRequestURI();
			}
			baseURL = baseURL+"?token="+token;
			
			if(notificationTimestamp!=0) {
				baseURL+="&notificationtimestamp="+notificationTimestamp;
			}
			resp.setHeader(HttpConst.X_BARCO_NOTIFICATION_CHANNEL, baseURL);
		}


		if(acceptHeader.endsWith(BaseHtppRequestHandler.JSON_MIME_SUFFIX) || acceptHeader.endsWith(BaseHtppRequestHandler.DEFAULT_MIME_SUFFIX) ) {			
			response = MRXMLUtils.xmlToJSONString(response, BaseHtppRequestHandler.JSON_INDENT);
		}		

		resp.setStatus(statusCode);	
		resp.getOutputStream().println(response);
		resp.getOutputStream().flush();
		//log.addInfo("writing response for URL:="+req.getRequestURL() +" queryString:="+req.getQueryString(),MRLog.INFO,MRLog.NOTIFY);
		((Request)req).setHandled(true);
	}

}