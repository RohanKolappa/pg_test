package com.ipvs.http.servlet;



import java.util.UUID;
import java.util.concurrent.Semaphore;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.MultivaluedMap;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;

import com.barco.utils.json.JSONException;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.MRClientAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.AuthTokenCache.UserResource;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

public abstract class BaseHtppRequestHandler {
	
	public static final long CONTINUATION_TIME_OUT = 500;
	public static final long CONTINUATION_EXPIRY_INTERVAL = 15000; //15 seconds 
			//(int)( (5*1000) / (CONTINUATION_TIME_OUT)); // should amount to 15 sec
	public static final String JSON_MIME_SUFFIX ="json";
	public static final String XML_MIME_SUFFIX = "xml";
	public static final String DEFAULT_MIME_SUFFIX = "*/*";
	public static final int JSON_INDENT = 4;
	
	protected static final String RESULTS ="results";
	protected XMPPI xmpp;
	protected MRLog log;
	protected MRDB db;
	protected MRResourceFactoryI resourceFactory;
	protected ServiceAgentFactoryI serviceAgentFactory;
	protected String userJID; 
	protected boolean requestAuthenticated  = false;
	
	public BaseHtppRequestHandler() {}		
	protected @Context HttpServletRequest servletRequest;
	protected Document getRequestDoc(MultivaluedMap<String, String> parameters, String requestName, String serviceName,Element requestElement,String protocol) throws Exception {
		String requestXML = "/com/ipvs/"+ serviceName.replace(".",  "/") + "/xml/" + requestName + "Request.xml";
		if(requestElement == null) {
			Document requestTemplate = MRXMLUtils.loadXMLResource(requestXML, log);
			requestElement = MRXMLUtils.getElementXpath(requestTemplate, "//" + requestName);
	        for(String key : parameters.keySet())  
			{	
				MRXMLUtils.setAttributeValue(requestElement, key, StringUtils.join(parameters.get(key), ","));
			}
		}
		Document doc = convertToActualRequestFormat(serviceName, requestName, requestElement.asXML());
		if(protocol == null || protocol.isEmpty())
			protocol = "http";
		MRXMLUtils.addElementXpath(doc, "//Header", "Protocol", protocol);
		
		return doc;		
	}
	
	
	protected Document callXMPPRequestHandler(String requestName, Document requestDoc, String serviceName) throws Exception {
		Document responseDoc = null;
		
		String handlerName = "com.ipvs." + serviceName + ".handler." + requestName + "RequestHandler";
		Class<?> c = Class.forName(handlerName);
		MRRequestHandlerI handler = (MRRequestHandlerI)c.newInstance();
		MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid());		
		//add admin check
		if(userJID.startsWith(MRServer.ADMIN+"@")) {
            request.setSuperUser(true);
        }
		responseDoc = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		return responseDoc;
		
	}
	
	protected UserResource validateUserAuthentication(HttpHeaders headers) throws Exception {
	    //List<String> authenticationHeader =  headers.getRequestHeader(HttpHeaders.AUTHORIZATION);
		AuthTokenCache authTokenCache = null;
		authTokenCache = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory);
		AuthTokenCache.UserResource userResource=null;
		
		try {
			userResource = authTokenCache.parseAuthHeader(headers);
		} catch (Exception e) {
			e.printStackTrace();
			log.addInfo("Error Validating User"+e.toString() ,MRLog.INFO,MRLog.NOTIFY);
		}
		
		if(userResource == null || !authTokenCache.validateUser(userResource)) {
				throw new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid user credentials");
		}
		
		this.userJID = userResource.getUserJID();
		return userResource;
		
	}
	
	protected Document convertToActualRequestFormat(String serviceName, String requestName, String request) throws Exception {
		String data = "<" + requestName + "RequestData>" + request + "</" + requestName + "RequestData>";
	    String requestStr = MRClientAgentUtils.formatRequestString(serviceName, requestName+"Request", userJID, "", "", AppServerAgent.class.getName(), data, "false");
	    return MRXMLUtils.stringToDocument(requestStr);
		
	}
	
	protected void setInfo(ServletContext servletContext) {
		this.serviceAgentFactory = (ServiceAgentFactoryI) servletContext.getAttribute("ServiceAgentFactoryI");
		this.xmpp = (XMPPI) servletContext.getAttribute("XMPPI");
		this.log = (MRLog) servletContext.getAttribute("MRLog");
		this.db = (MRDB) servletContext.getAttribute("MRDB");
		this.resourceFactory = (MRResourceFactoryI) servletContext.getAttribute("MRResourceFactoryI");
	}
	
	
	
	
	protected void handleHttpRequest(ServletContext servletContext,UriInfo uriInfo,HttpHeaders headers,String requestName,Element requestElement,final @Suspend(1000) AsynchronousResponse asynResponse) {
		setInfo(servletContext);
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		if(acceptHeader == null) {
            acceptHeader = DEFAULT_MIME_SUFFIX;
        } 
		if(!requestAuthenticated) {		
			try {
				validateUserAuthentication(headers);
			} catch (Exception e1) {
			    Response res =  Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(acceptHeader).entity(e1.toString()).build();
			    asynResponse.setResponse(res);
                return;
			}
		}
		
		try {
			long startTime = System.currentTimeMillis();
			log.addInfo("Http handleRequest start: requestName :="+requestName,MRLog.INFO,MRLog.NOTIFY);
			String serviceName = SessionServiceUtils.getServiceName(requestName+"Request", log);
			Document requestDoc = getRequestDoc(uriInfo.getQueryParameters(), requestName, serviceName,requestElement,servletRequest.getHeader("X-Forwarded-Proto"));
			String clientData = UUID.randomUUID().toString();
	        MRXMLUtils.setValueXpath(requestDoc,"//ClientData", clientData); 
			Document responseDoc = callXMPPRequestHandler(requestName, requestDoc, serviceName);
			
			if (responseDoc == null){
		            Continuation cc = ContinuationSupport.getContinuation(servletRequest);
		            cc.setTimeout(10000); //setting timeout as 10 sec.
		            cc.suspend();
		            AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).addRequestToHttpCache(clientData, cc); 
		            servletRequest.setAttribute("clientdata", clientData); 
		            return;
		        }
			 else{
			String responseStr = getResponseString(responseDoc,requestName);
			if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
				responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
			}
			long duration = System.currentTimeMillis() - startTime;
			Response response =  Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build();
			log.addInfo("Http handleRequest End:  requestName :="+requestName + " duration=" + duration + " URL=" + uriInfo.getRequestUri(),MRLog.INFO,MRLog.NOTIFY);
			//TBD: time taken to process the request
			asynResponse.setResponse(response);
            return;
			 }
		    
		} catch (Exception e) {
			log.addInfo("Http handleRequest Failed: requestName :="+requestName,MRLog.INFO,MRLog.NOTIFY);
			e.printStackTrace();
			String responseStr = e.toString();
			if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
			    try{
			    	responseStr = MRXMLUtils.xmlToJSONString(e.toString(), JSON_INDENT);
			    }catch(JSONException jsone){
			      log.addInfo("Error response is not in xml format");
			    }
                
            }
			Response res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(responseStr).build();
			asynResponse.setResponse(res);
	        return;
		}		
	}
	
	protected abstract String getResponseString(Document responseDoc,String requestName)  throws Exception ; 
	
	protected Response handleUserUnavailable(HttpHeaders headers, ServletContext servletContext, boolean kick)  throws Exception {
		AuthTokenCache authTokenCache = null;
		AuthTokenCache.UserResource userResource = null;
		Semaphore lock = null;
		
		try {
			authTokenCache = AppServerAgentUtils.getAuthTokenCache((ServiceAgentFactoryI)servletContext.getAttribute("ServiceAgentFactoryI"));
			userResource = validateUserAuthentication(headers);
		
			//Acquire lock on user jid
			lock = IPVSMonitor.getMonitor().getLock(userResource.getUserJID(), userResource.getUserJID());				
		    log.addInfo("handleUserUnavailable got lock for userJID=" + userResource.getUserJID(), MRLog.OK, MRLog.NOTIFY);
			
			String userName = userResource.getUsername();
			if(authTokenCache.deleteUserFromCache(userResource, kick)){
				AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getResourcePresenceManager().updateResourceContactAndState(userResource.getUserJID(),true, AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log,xmpp,resourceFactory,serviceAgentFactory);
				return Response.ok().type(MediaType.TEXT_HTML).entity(userName + ": User removed successfully!").build();
			}
			return Response.ok().type(MediaType.TEXT_HTML).entity("User not present in the cache").build();	
		
		} catch (Exception e) {
			e.printStackTrace();
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(MediaType.TEXT_HTML).entity(e.toString()).build();
		}	finally {
			if(lock!=null && userResource!=null) {
				IPVSMonitor.getMonitor().releaseLock(userResource.getUserJID(), lock);
				log.addInfo("handleUserUnavailable release lock  for userJID=" + userResource.getUserJID(), MRLog.OK, MRLog.NOTIFY);
			}
		}
	} 
	
  protected void handleRedispatchedRequest(Continuation cc, ServletContext servletContext, String acceptHeader, final @Suspend(1000) AsynchronousResponse asynResponse){
      
      String responseStr = "";
      Response res =null;
          
     
      try{
              if(cc.isExpired()){
              responseStr = "Request has been redispatched as the result of a timeout !" ;
              res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(responseStr).build();
              asynResponse.setResponse(res);
              AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).removeRequestFromCache(cc);
              return;
              }
              
              responseStr = (String)cc.getAttribute("result");
              
              if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
            	  responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
              }
              
              String state = (String)cc.getAttribute("REQUEST_STATE");
              
              // checking state=8 for legacy XMPP support. TODO: Remove state=8 check
              if(state.equals("200") || state.equals("8")){
              res = Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build();
              }else{
              res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(responseStr).build();   
              }
              asynResponse.setResponse(res);
              AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).removeRequestFromCache(cc);
              return;
          
      }catch(Exception e){
          e.printStackTrace();
          res = Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(responseStr).build();
          asynResponse.setResponse(res);
          return;
      }
      
  }
  
  protected Continuation getCachedHttpContinuation(HttpServletRequest servletRequest, String acceptHeader)throws Exception{
      Continuation cc =null;
      String clientdata =(String)servletRequest.getAttribute("clientdata");
      if(clientdata == null || clientdata.isEmpty()) {
        return cc;
    }
      cc = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).getHttpRequestCache().get(clientdata);
      if(cc != null && (cc.isResumed() || cc.isExpired())){ //if resumed or timedOut
          return cc;
     }else{
          return cc;
      }
  }
}
