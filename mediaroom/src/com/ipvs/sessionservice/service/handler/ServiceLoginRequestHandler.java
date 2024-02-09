package com.ipvs.sessionservice.service.handler;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.util.UrlEncoded;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.AuthTokenCache.UserResource;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpConst;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.handler.GetMediaRoomInvitesRequestHandler;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.handler.SetServiceDomainRequestHandler;

@Path("/Service/Login")
public class ServiceLoginRequestHandler extends BaseHtppRequestHandler implements MRRequestHandlerI{

	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;
	protected static final String REQUESTNAME = "ServiceLogin";

	@POST
	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo,
			@Context HttpHeaders headers) throws Exception{
		setInfo(servletContext);
		Document responseDoc = null;
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		if(acceptHeader == null)
			acceptHeader = DEFAULT_MIME_SUFFIX;
		AuthTokenCache.UserResource userResource = null;
		AuthTokenCache authTokenCache = null;
		
		try {
			
			authTokenCache = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory);
			
			try {
				userResource = authTokenCache.parseAuthHeader(headers);
			} catch (Exception e) {
				e.printStackTrace();
				log.addInfo("Error Validating User"+e.toString() ,MRLog.INFO,MRLog.NOTIFY);
			}

			//if user already logged in then return conflict error			
			if (userResource != null && userResource.isPresenceAvailable()) {
				boolean foundUserInCache = false;				
				
				if(userResource.getPassword().equals(userResource.getPassword())) {
					foundUserInCache=true;
				} else {
					throw new MRException(HttpServletResponse.SC_UNAUTHORIZED,"Invalid User Credentials");
				}
				
				if(foundUserInCache) {
					return Response.status(HttpServletResponse.SC_CONFLICT).entity("User Already Loggedin").build();
				}
			}
						
			if(userResource == null || !authTokenCache.validateUser(userResource)) {
				throw new MRException(HttpServletResponse.SC_UNAUTHORIZED, "Authentication failed. Invalid user credentials");
			}
			userJID = userResource.getUserJID();
			
		} catch (Exception e1) {
			log.addInfo("ServiceLogin Failed! : Authorization " + e1.toString());
			return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(acceptHeader).entity(e1.toString()).build();
		}
		Semaphore lock = null;
		try {
			
			//aquire lock on user jid 
			lock = IPVSMonitor.getMonitor().getLock(userResource.getUserJID(), userResource.getUserJID());				
	        log.addInfo("ServiceLoginRequestHandler:getResponse got lock for userJID=" + userResource.getUserJID(), MRLog.OK, MRLog.NOTIFY);
			
			Document requestBody = null;
			try {
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					xml = MRXMLUtils.jsonToXMLString(xml);
				}

				//convert to actual request
				requestBody = convertToActualRequestFormat(ServiceClient.SESSIONSERVICE_SERVICE, REQUESTNAME, xml);
				MRXMLUtils.setAttributeValueXPath(requestBody, "//ServiceLogin", "ready", "true");
			} catch (Exception e) {
				log.addInfo("ServiceLoginRequestHandler: Service Login Failed. Bad Request " + e.toString());
				return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(e.toString()).build();
			}
			MRRequest request = MRRequest.createRequest(userJID, requestBody, xmpp.getXMPPUserJid());
			responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
			if(responseDoc != null) {
				String responseStr = responseDoc.asXML();
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
				}

				//Set presenceAvailable flag to true
				userResource.setPresenceAvailable(true);

				// create the continuation url
				AppServerAgent agent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
				Element ApiKeyTableEl = agent.getAPIKey();
				String msIPAddress = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.MS_IPADDRESS, "");

				String baseURL = uriInfo.getBaseUri().toString().trim();
				String scheme = servletRequest.getHeader("X-Forwarded-Proto");
				
				if(scheme == null || scheme.isEmpty()){
					scheme = "http";
				}
				String forwardedHost =servletRequest.getHeader("X-Forwarded-Host");
				if(forwardedHost != null) {
					log.addInfo("HTTPLoginRequest: forwardedHost:="+forwardedHost,MRLog.INFO,MRLog.NOTIFY);
					msIPAddress = forwardedHost;
				}
				
				if(msIPAddress != null){
				 baseURL = scheme + "://" + msIPAddress  + servletRequest.getRequestURI();
				}
				baseURL = baseURL.replace("/Service/Login", "/Service/NotificationChannel");

				baseURL = baseURL +"?token=" + UrlEncoded.encodeString(userResource.getToken());
				Response response = Response.status(HttpServletResponse.SC_OK).header(HttpConst.X_BARCO_NOTIFICATION_CHANNEL, baseURL).type(acceptHeader).entity(responseStr).build(); 
				return response;
			}
			return Response.status(HttpServletResponse.SC_NO_CONTENT).type(acceptHeader).entity("ServiceLogin Failed! No Content found!").build();

		} catch (MRException e) {
			log.addInfo("ServiceLoginRequestHandler: Service Login Failed. " + e.toString());
			e.printStackTrace();
			
			if(MRExceptionCodes.DEVICE_ALREADY_IN_USE != e.getCode()  && authTokenCache.deleteUserFromCache(userResource, false)){
				AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getResourcePresenceManager().updateResourceContactAndState(userResource.getUserJID(),true, AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log,xmpp,resourceFactory,serviceAgentFactory);
			}
			
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build();
		} catch(Exception e) {
			log.addInfo("ServiceLoginRequestHandler: Service Login Failed. " + e.toString());
			e.printStackTrace();
			
			if(authTokenCache.deleteUserFromCache(userResource, false)){
				AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getResourcePresenceManager().updateResourceContactAndState(userResource.getUserJID(),true, AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log,xmpp,resourceFactory,serviceAgentFactory);
			}
			
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build();
		} finally {
			IPVSMonitor.getMonitor().releaseLock(userResource.getUserJID(), lock);
		    log.addInfo("ServiceLoginRequestHandler:getResponse release lock for userJID=" + userResource.getUserJID(), MRLog.OK, MRLog.NOTIFY);
			
		}

	}



	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);

		return auth;
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document requestDoc = request.getMessage().getDocument();
		String clientVersion = MRXMLUtils.getAttributeValueXpath(requestDoc, "//ServiceLogin","clientVersion",null);
		// String clientContact = MRXMLUtils.getValueXpath(requestDoc, "//clientContact", "");
		// String clientState = MRXMLUtils.getValueXpath(requestDoc, "//clientState", "");
		String clientPrivateKey = MRXMLUtils.getValueXpath(requestDoc, "//clientPrivateKey", "");
		String clientTemplateTitle = MRXMLUtils.getValueXpath(requestDoc, "//clientTemplateTitle", "");

		String readyValue = MRXMLUtils.getAttributeValueXpath(requestDoc, "//ServiceLogin","ready",null);
		boolean ready = "true".equals(readyValue);

		Document responseDoc = this.requestSetServiceDomain(request, xmpp, log, db, 
				resourceFactory, serviceAgentFactory, 
				clientTemplateTitle, clientVersion, clientPrivateKey);

		if(ready) {
			this.sendUpdateStatusAndGetInvitesRequestsToAppServer(request, xmpp, log, db, 
					resourceFactory, serviceAgentFactory, responseDoc);
		}
		// System.out.println(MRXMLUtils.documentToString(responseDoc));
		String serverDate = String.valueOf(xmpp.getCurrentTimeMillis());
		Element assetPrivateData = MRXMLUtils.getElementXpath(responseDoc, "//AssetPrivateData");
		Element clientPrivateData = (Element)assetPrivateData.clone();
		clientPrivateData.setName("clientPrivateData");

		Document doc = MRXMLUtils.loadXMLResource(SessionServiceUtils.SERVICELOGIN_RESPONSE_XML, log);
		Element serviceLoginElement = MRXMLUtils.getElementXpath(doc, "//ServiceLogin");
		MRXMLUtils.getElementXpath(doc, "//serverDate").setText(serverDate);
		MRXMLUtils.removeChild(serviceLoginElement,"clientPrivateData");
		serviceLoginElement.add(clientPrivateData);
		return request.createResponseData((Element)doc.getRootElement().detach());

	}

	public Document sendUpdateStatusAndGetInvitesRequestsToAppServer(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document setServiceDomainResponseDoc) throws Exception {
		MRRequestHandlerI handler = new GetMediaRoomInvitesRequestHandler();
		Document requestDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.GET_MEDIAROOM_INVITES_XML, log);

		MRXMLUtils.setValueXpath(requestDoc, "//contactJID", request.getUserJID());


		String clientVersion = MRXMLUtils.getAttributeValueXpath(request.getMessage().getDocument(), "//ServiceLogin","clientVersion",null);
		Element serviceVersionEl = MRXMLUtils.newElement(MRConst.SERVICE_VERSION);
		serviceVersionEl.setText(clientVersion);
		MRXMLUtils.addElement(requestDoc.getRootElement(), serviceVersionEl);

		Element resourceNIDListElement = MRXMLUtils.getElementXpath(requestDoc, "//resourceNIDList");

		List<String> resourceNIDList = new ArrayList<String>();
		Document assetResourceDoc = MRXMLUtils.getDocumentXpath(setServiceDomainResponseDoc, "//" + AMConst.AssetResource);
		if (assetResourceDoc != null) {
			String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), "NID", null);
			resourceNIDList.add(assetResourceNID);

			if (assetResourceNID != null) {
				MRXMLUtils.addElement(resourceNIDListElement, MRXMLUtils.stringToElement("<resourceNID>" + assetResourceNID + "</resourceNID>"));
			}
		}
		Element portResourcEList = MRXMLUtils.getElementXpath(setServiceDomainResponseDoc, "//PortResourceNIDList");
		List<?> listOfPorts = MRXMLUtils.getListXpath(portResourcEList, "resourceNID");
		for (Object object : listOfPorts) {
			resourceNIDList.add(MRXMLUtils.getValue((Element) object));
		}

		for (String resourceNID : resourceNIDList) {
			MRXMLUtils.addElement(resourceNIDListElement, MRXMLUtils.stringToElement("<resourceNID>" + resourceNID + "</resourceNID>"));
		}

		String serviceName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
		String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
		MRMessage message = new MRMessage(requestDoc);
		String resourceGroup = "";
		request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
		Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		return response;
	}
	public Document requestSetServiceDomain(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String clientTemplateTitle, String clientVersion, String clientPrivateKey) throws Exception {

		MRRequestHandlerI handler = new SetServiceDomainRequestHandler();
		Document requestDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.SET_SERVICE_DOMAIN_XML, log);

		MRXMLUtils.setValueXpath(requestDoc, "//AssetTemplateTitle",clientTemplateTitle);
		MRXMLUtils.setValueXpath(requestDoc, "//ServiceVersion",clientVersion);
		MRXMLUtils.setValueXpath(requestDoc, "//AssetPrivateKeyData",clientPrivateKey);
		String serviceName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
		String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");

		MRMessage message = new MRMessage(requestDoc);
		String resourceGroup = "";
		request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
		Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);

		return response;
	}

	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		return ""; // Do nothing.
	}


}
