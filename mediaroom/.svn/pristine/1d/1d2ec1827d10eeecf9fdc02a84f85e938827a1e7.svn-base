package com.ipvs.sessionservice.service.handler;


import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;
import org.jboss.resteasy.util.HttpResponseCodes;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.utils.Utils;

@Path("/Service/NotificationChannel")
@Deprecated

public class NotificationChannelRequestHandler extends BaseHtppRequestHandler{

	public NotificationChannelRequestHandler() {

	}

	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;
	

	@GET
	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
	public void handleGetRequest(@Context UriInfo uriInfo, @Context HttpHeaders headers, @QueryParam("token") String token,@QueryParam("expiryinterval") String expiryInterval, final @Suspend(1000) AsynchronousResponse asynResponse) throws MRException{     
		setInfo(servletContext);
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		if(token == null) {
			log.addInfo("NotificationChannelRequestHandler: handleGetRequest token:="+token,MRLog.INFO,MRLog.NOTIFY);			
			Response response = Response.status(HttpResponseCodes.SC_UNAUTHORIZED).entity(new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid token").toString()).build();
			asynResponse.setResponse(response);
			return;
		}
		
		AuthTokenCache authTokenCache=null;
		AuthTokenCache.UserResource userResourceFromCache=null;
		AuthTokenCache.UserResource parsedUserResource=null;
		try {
			authTokenCache = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory);
			parsedUserResource = authTokenCache.parseAuthHeader(headers);
		} catch (Exception e1) {
			//ignore exception
			log.addInfo("NotificationChannelRequestHandler: Exception occur while finding the auth cache:"+MRException.getStackTrace(e1),MRLog.INFO,MRLog.NOTIFY);
		}
		
		Continuation requestContinuation = null;
		try {
			requestContinuation = ContinuationSupport.getContinuation(servletRequest);
		} catch(Exception e) {
			e.printStackTrace();
			Response response = Response.status(HttpResponseCodes.SC_INTERNAL_SERVER_ERROR).entity(e.toString()).build();
			asynResponse.setResponse(response);
			return;
		}
		
		String emptyNotification = "<NotificationList/>";
		if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
			emptyNotification = MRXMLUtils.xmlToJSONString(emptyNotification, JSON_INDENT);
		}
		
		// Check if this is a kick out or usual logout
		if(requestContinuation != null && requestContinuation.getAttribute("kick") != null) {
			String kick = (String)requestContinuation.getAttribute("kick");
			Document notificationDoc = null;
			try {
				notificationDoc = MRXMLUtils.stringToDocument("<NotificationList/>");
				String statusStr = (kick.equalsIgnoreCase("true")) ? "KickedOut" : "LoggedOut";
				log.addInfo("NotificationChannelRequestHandler: User: " + statusStr + " : " + parsedUserResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				MRXMLUtils.addElement(notificationDoc.getRootElement(), statusStr, "");
				String notification = notificationDoc.asXML();
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					notification = MRXMLUtils.xmlToJSONString(notification, JSON_INDENT);
				}
				
				Response response = Response.status(HttpResponseCodes.SC_OK).entity(notification).build();
	    		asynResponse.setResponse(response);
				log.addInfo("End NotificationChannel For user =" + parsedUserResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				return;
			} catch (Exception e) { }
		}		
		
		// Get the userResource from cache.
		userResourceFromCache = authTokenCache.getUserResource(parsedUserResource.getUsername());
		if(userResourceFromCache == null) {
			log.addInfo("NotificationChannelRequestHandler: Invalid User.",MRLog.INFO,MRLog.NOTIFY);
		   	Response response = Response.status(HttpResponseCodes.SC_UNAUTHORIZED).entity(new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid user").toString()).build();
			asynResponse.setResponse(response);
			return;
		}
		
		// Verify the token matches the cached token for user.
		if(!userResourceFromCache.getToken().equals(token)) {
			log.addInfo("NotificationChannelRequestHandler: Unauthorized token:="+token,MRLog.INFO,MRLog.NOTIFY);
		   	Response response = Response.status(HttpResponseCodes.SC_UNAUTHORIZED).entity(new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid token").toString()).build();
			asynResponse.setResponse(response);
			return;
		}
		
		try {
			Continuation cachedContinuation = userResourceFromCache.getContinuation();
			
			//this will handle the case where user started long poll and ablort the request 
			if(cachedContinuation != null && !requestContinuation.equals(cachedContinuation)){
				log.addInfo("Complete Existing Contiuation for user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				try {
					cachedContinuation.complete();
				}
				catch(Exception e) {
					log.addInfo("Complete Existing Contiuation for user =" + userResourceFromCache.getUsername() + " error=" + e.toString(),MRLog.INFO,MRLog.NOTIFY);					
				}
				cachedContinuation = null;
			}
			
			if(cachedContinuation == null) {				
				log.addInfo("Initializing the NotificationChannel for user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				cachedContinuation = requestContinuation;
				//userResource.setContinuationRetryCount(retryCount+1);

				authTokenCache.addContinuationToUserResource(userResourceFromCache.getUsername(), requestContinuation);
				userResourceFromCache.setContinuationExpiryTime(System.currentTimeMillis() + Utils.getLongValue(expiryInterval, CONTINUATION_EXPIRY_INTERVAL));
			} 
			
			cachedContinuation.setTimeout(CONTINUATION_TIME_OUT);
			
			//write result
			String notification  = userResourceFromCache.getNotifcations();
			cachedContinuation.suspend();

			if(notification != null) {
				
				log.addInfo("Found Notification For user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					notification = MRXMLUtils.xmlToJSONString(notification, JSON_INDENT);
				}
				
				Response response = Response.status(HttpResponseCodes.SC_OK).entity(notification).build();
				asynResponse.setResponse(response);
				userResourceFromCache.setContinuation(null);
				cachedContinuation.complete();
				log.addInfo("End NotificationChannel For user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				return;
			}			
		
			if(!cachedContinuation.isInitial()) {
				//check continuation expired
				if(System.currentTimeMillis() >= userResourceFromCache.getContinuationExpiryTime()) {					
 					Response response = Response.status(HttpResponseCodes.SC_OK).entity(emptyNotification).build();
					asynResponse.setResponse(response);
					userResourceFromCache.setContinuation(null);
					cachedContinuation.complete();
					log.addInfo("End NotificationChannel For user =" + userResourceFromCache.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				}

			} 
		} catch (Exception e) {
			e.printStackTrace();
			Response response = Response.status(HttpResponseCodes.SC_INTERNAL_SERVER_ERROR).entity(e.toString()).build();
			asynResponse.setResponse(response);
		}

	}

	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		return "";
	}

}
