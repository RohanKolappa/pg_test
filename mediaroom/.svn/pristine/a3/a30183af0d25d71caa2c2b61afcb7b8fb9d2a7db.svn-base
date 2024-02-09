package com.ipvs.sessionservice.service.handler;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.util.StringUtils;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionObjectPermissions;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
@Path("/Service/ServiceClient")
public class GetServiceClientRequestHandler extends BaseHtppRequestHandler implements MRRequestHandlerI{
	
	private static final String GET_SERVICE_CLIENT = "GetServiceClient";  
	   	 
	   	@Context ServletContext servletContext;
	   	@Context HttpServletRequest servletRequest;
	  	@GET 
	  	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML}) 
	   	public Response handleGetRequest(@Context UriInfo uriInfo, @Context HttpHeaders headers) throws MRException{      
	   		setInfo(servletContext); 
	   		Document responseDoc = null; 
	   		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0); 
	   		if(acceptHeader == null) {
                acceptHeader = DEFAULT_MIME_SUFFIX;
            } 
	   		
	   		try {
				validateUserAuthentication(headers);
			} catch (Exception e1) {
				log.addInfo("GetServiceClient Failed! : Authorization " + e1.toString());
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(e1.toString()).build();
			}
	   		 
	   		try { 
	   			Document requestDoc =null; 
	   			try { 
	   				String serviceName = SessionServiceUtils.getServiceName(GET_SERVICE_CLIENT+"Request", log); 
	   				requestDoc = getRequestDoc(uriInfo.getQueryParameters(), GET_SERVICE_CLIENT, serviceName, null, servletRequest.getHeader("X-Forwarded-Proto")); 
	   			} catch (Exception e) { 
	   				log.addInfo("GetServiceClientRequestHandler: GetServiceClient Failed. Bad Request " + e.toString()); 
	   				return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(e.toString()).build(); 
	  			} 
	   			MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid()); 
	   			responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory); 
	   			if(responseDoc != null) { 
	   				String responseStr = responseDoc.asXML(); 
	   				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) { 
	   					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
	   				} 
	   				Response response =  Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build(); 
	   				return response; 
	   			} 
	   			return Response.status(HttpServletResponse.SC_NO_CONTENT).type(acceptHeader).entity("GetServiceClient Failed! No Content found!").build(); 
	    
	   		} catch (Exception e) { 
	   			log.addInfo("GetServiceClientHandler: GetServiceClient Failed. " + e.toString()); 
	   			e.printStackTrace(); 
	   			 
	   			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build(); 
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
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document requestDoc = request.getMessage().getDocument();
		Element getServiceClientReqEl  = MRXMLUtils.getElementXpath(requestDoc, "//"+GET_SERVICE_CLIENT);
		String contactJID = MRXMLUtils.getAttributeValue(getServiceClientReqEl, SessionServiceConst.WHERE_CONTACT_JID,"");		
		boolean isSuperuser = request.isSuperUser();
		if(contactJID.isEmpty() || contactJID.equals(request.getUserJID())){
			contactJID = request.getUserJID();
			//by pass the policy check for logged in user if user is asking for own info
			request.setSuperUser(true);			
		} 
		 if (StringUtils.parseName(contactJID).length() <= 0
                || StringUtils.parseServer(contactJID).length() <= 0
                || StringUtils.parseResource(contactJID).length() <= 0) {
		     throw new MRException(AMExceptionCodes.VALID_ERROR_CONTACT_JID, MRXMLUtils.generateErrorXML("contactJID", contactJID));
        }
	          
		String selectFormat = MRXMLUtils.getAttributeValue(getServiceClientReqEl, SessionServiceConst.SELECTFORMAT,"");
	    String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.SERVICE_CLIENT + SessionServiceConst.XQUERYXML;	    	   
	    String deviceXQueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.DEVICE + SessionServiceConst.XQUERYXML;	    
		
	    // Combine DeviceQuery.xml with ServiceClientQuery.xml since the declaration for getPortList
	    // and getDeviceInfo are present in DeviceQuery.xml, which are used by ServiceClientQuery.xml
	    String query = FileUtils.readFileResource(deviceXQueryFileName, request.getClass());
		query = query + FileUtils.readFileResource(xqueryFileName, request.getClass());
		
		query = query.replace(SessionServiceConst.DETAIL, selectFormat);
		
		// We need to manually replace the following constants from DeviceQuery.xml since it is not being passed
		// to GetObjectUtils. If we don't replace the constants the query would fail. 
		query = query.replace(SessionServiceConst.XPATHAND, "");		
		query = query.replace(SessionServiceConst.SORTBY, "");
		query = query.replace(SessionServiceConst.SORTORDER, "");
		
		query = query.replace(SessionServiceConst.USER_JID, contactJID);		
		Document queryResultDoc = GetObjectUtils.executeQuery(query, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
	
		if(!isSuperuser) {			
			request.setSuperUser(false);	
		}
		//update permission element 
		MRXMLUtils.addElement(queryResultDoc.getRootElement(),SessionObjectPermissions.getViewObjectsPermissions(request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		Document responseDocument = request.createResponseData();
		MRXMLUtils.addElement(responseDocument.getRootElement(), (Element)queryResultDoc.getRootElement().detach());		
		return responseDocument;
	}
	@Override
	protected String getResponseString(Document responseDoc, String requestName) throws Exception {
		// TODO Auto-generated method stub
		return "";
	}

}
