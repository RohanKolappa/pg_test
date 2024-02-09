package com.ipvs.sessionservice.session.handler;


import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.sessionservice.impl.SessionManager;

@Path("/Session")
public class HttpSessionRequestHandler extends BaseHtppRequestHandler{

	
	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;

	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		if(responseDoc==null){
			return "";
		}
		Element respEl = MRXMLUtils.getElementXpath(responseDoc, "//"+requestName);
		Document doc = null;
		if(respEl == null){
			doc = DocumentHelper.createDocument();
			Element rootEl = doc.addElement(requestName);
			MRXMLUtils.replaceContent(rootEl, responseDoc.getRootElement());
			return doc.asXML();
		} else {
			return responseDoc.asXML();
		}
	}	
	

	@POST  
    @Consumes({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
    @Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
	public void handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers,@QueryParam("NID") String byNID,@QueryParam("title") String title,
	        @QueryParam("sessionId") String sessionId, final @Suspend(1000) AsynchronousResponse asynResponse) {
	        setInfo(servletContext);
	        String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
	        if(acceptHeader == null) {
	            acceptHeader = DEFAULT_MIME_SUFFIX;
	        }
	       
	        try {
	            Continuation cc = getCachedHttpContinuation(servletRequest,acceptHeader);
	            if(cc != null) {
                    super.handleRedispatchedRequest(cc, servletContext,acceptHeader,asynResponse);
                    return;
                }
	            
	            if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
	            	xml = MRXMLUtils.jsonToXMLString(xml);
	            }   Document requestBody = MRXMLUtils.stringToDocument(xml);        
	            String requestName = requestBody.getRootElement().getName();
	            
	            
	            AuthTokenCache authTokenCache = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory);
	            AuthTokenCache.UserResource userResource=null;
	            
	            try {
	                userResource = authTokenCache.parseAuthHeader(headers);
	            } catch (Exception e) {
	                log.addInfo("HttpSesssionRequestHandler: Authorization header not found, look for session information." ,MRLog.INFO,MRLog.NOTIFY);
	            }
	            
	            if(userResource == null) {
	                // authorization header  not provided. validate user with session information
	                SessionManager sessionManager = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getSessionManager();
	                try {
	                    Document sessioDoc = sessionManager.getSessionDoc(MRRequest.createRequest(userJID), xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);                     
	                    this.userJID = MRXMLUtils.getAttributeValueXpath(sessioDoc, "//Properties", "ownerUserJID", "");;
	                    requestAuthenticated = true;
	                } catch (Exception e) {
	                    e.printStackTrace();
	                    Response res =  Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(acceptHeader).entity(
	                                       new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid SessionId").toString()).build(); 
	                    asynResponse.setResponse(res);
	                    return;
	                    
	                }
	            }
	            
	            super.handleHttpRequest(servletContext, uriInfo, headers, requestName ,requestBody.getRootElement(), asynResponse);
	          
	        } catch (Exception e) {
	            log.addInfo("Http handleRequest Failed:",MRLog.INFO,MRLog.NOTIFY);
	            e.printStackTrace();
	            Response res =   Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build();
	            asynResponse.setResponse(res);
	            return;
	        }
	    }


}
