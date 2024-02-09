package com.ipvs.sessionservice.service.handler;


import javax.servlet.ServletContext;
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

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.SessionServiceConst;

@Path("/Service/AuditEvent")
public class AuditEventRequestHandler extends BaseHtppRequestHandler implements MRRequestHandlerI {
	
	@Context ServletContext servletContext;
	
	@POST
	@Produces({ MediaType.APPLICATION_XML, MediaType.APPLICATION_JSON, MediaType.TEXT_XML })
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo,
			@Context HttpHeaders headers) throws Exception{
		setInfo(servletContext);
		Document responseDoc = null;
		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0);
		if(acceptHeader == null)
			acceptHeader = DEFAULT_MIME_SUFFIX;
		AuthTokenCache.UserResource userResource = null;
		
		try {
			userResource = validateUserAuthentication(headers);
		} catch (Exception e1) {
			log.addInfo("AuditEvent Failed! : Authorization " + e1.toString());
			return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(e1.toString()).build();
		}
		
		try {
			Document requestBody = null;
			try {
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					xml = MRXMLUtils.jsonToXMLString(xml);
				}
				
				//convert to actual request
				 requestBody = convertToActualRequestFormat(ServiceClient.SESSIONSERVICE_SERVICE, "AuditEvent", xml);
			} catch (Exception e) {
				log.addInfo("AuditEventRequestHandler: Audit Event Failed. Bad Request " + e.toString());
				return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(MediaType.TEXT_XML).entity(e.toString()).build();
			}
			
			MRRequest request = MRRequest.createRequest(userJID, requestBody, xmpp.getXMPPUserJid());
			responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
			if(responseDoc != null) {
				String responseStr = responseDoc.asXML();
				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) {
					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
				}
				
				Response response = Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build(); 
				return response;
			}
			return Response.status(HttpServletResponse.SC_NO_CONTENT).type(MediaType.TEXT_XML).entity("AuditEvent Failed! No Content found!").build();
			
		} catch (Exception e) {
			log.addInfo("AuditEventRequestHandler: Audit Event Failed. " + e.toString());
			e.printStackTrace();
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(MediaType.TEXT_XML).entity(e.toString()).build();
		}
	}
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

	    int authType = MediaRoomObject.ROOM_TYPEID;
	    int role = MediaRoomPEM.VISITOR_ROLE;
	    String token = "testCommonToken";
	    String authority = "testAuthority";
	    String roomPolicyNID = MRConst.NULL;
	    String streamPolicyNID = MRConst.NULL;
	    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
	                authority);
	    log.addTraceEnd(request);

	    return auth;
	}

     
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	Document requestDoc = request.getMessage().getDocument();
    	    	    	
    	// Retrieve the values from the request element
    	String type = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + SessionServiceConst.AUDIT_EVENT_EL_NAME, SessionServiceConst.TYPE_ATTRIBUTE, "");    	
    	String level = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + SessionServiceConst.AUDIT_EVENT_EL_NAME, SessionServiceConst.LEVEL, "");
    	String name = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + SessionServiceConst.AUDIT_EVENT_EL_NAME, SessionServiceConst.NAME, "");
    	String target = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + SessionServiceConst.AUDIT_EVENT_EL_NAME, SessionServiceConst.TARGET, "");
    	String status = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + SessionServiceConst.AUDIT_EVENT_EL_NAME, SessionServiceConst.STATUS, "");
    	
    	if(level.isEmpty())
    		  level = AuditEventLogger.LEVEL_INFO;
    	Element descriptionEl = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.DESCRIPTION);
    	Element eventXMLEl = MRXMLUtils.newElement("EventXML");
    	MRXMLUtils.addChildrenToElement(eventXMLEl, descriptionEl);
    	String descriptionText = MRXMLUtils.getAttributeValue(descriptionEl, SessionServiceConst.TEXT,"");
    	AuditEvent auditEvent = new AuditEvent.Builder( type, name, request.getUserJID(), level).descriptionText(descriptionText)
    	.descriptionXML(eventXMLEl).target(target).status(status).build();
    	AuditEventLogger.getInstance().addAuditEventToQueue(auditEvent);
    	Document responseDoc = request.createResponseData(MRXMLUtils.stringToElement("<" + SessionServiceConst.AUDIT_EVENT_EL_NAME + "/>"));
    	
    	return responseDoc;
    }

	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		// TODO Auto-generated method stub
		return "";
	}
        
}