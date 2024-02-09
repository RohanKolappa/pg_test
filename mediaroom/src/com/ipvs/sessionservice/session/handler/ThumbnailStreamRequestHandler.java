package com.ipvs.sessionservice.session.handler;




import javax.jws.WebService;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;
import org.jboss.resteasy.util.HttpResponseCodes;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.http.common.HttpAuthentication.AuthScheme;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;

@Path("/Thumbnail")
@WebService(name="ThumbnailStreamRequestHandler")
public class ThumbnailStreamRequestHandler extends BaseHtppRequestHandler{

	public ThumbnailStreamRequestHandler() {
		
	}
	
	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;
	
	@POST
	public void handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers, @QueryParam("NID") String sourceNID, @QueryParam("size") String size, @QueryParam("action") String action, @QueryParam("type") String type,final @Suspend(5000) AsynchronousResponse asynResponse) throws MRException{
		setInfo(servletContext);
		Response response = Response.status(HttpResponseCodes.SC_ACCEPTED).type(MediaType.TEXT_XML).entity("Accepted Thumbnail Request!").build();
		asynResponse.setResponse(response);
	
		try {
			
			String requestXML = "<" + action + "PostingThumbnailRequest />";//<"+ action + "PostingThumbnail /></"+ action + "PostingThumbnailRequest>";
			Document requestDoc = MRXMLUtils.stringToDocument(requestXML);
			//	String protocol = servletRequest.getHeader("X-Forwarded-Proto");
		//	if(protocol == null || protocol.isEmpty())
		//		protocol = "http";
		//	if (!publishURL.startsWith(protocol + ":")) {
		//		String s = publishURL.substring(publishURL.indexOf(":"));
		//		publishURL = protocol + s;
		//	}
			
			String requestName = requestDoc.getRootElement().getName();
			
			Document sourceDoc;
			String portId;
			String deviceJID;
			String dstTitle = null; 
			MRRequest request = MRRequest.createRequest(userJID);
			if("live".equals(type.trim())){
				sourceDoc = MRXMLDBUtils.readElement(request, log, db, sourceNID);
				portId = MRXMLUtils.getValueXpath(sourceDoc,"//"+SessionServiceResourceConst.DEVICE_ELEMENT_ID_ELEMENT, "");
				deviceJID = MRXMLUtils.getValueXpath(sourceDoc, "//"+AMConst.OWNERUSERJID, "");
				MRXMLUtils.addAttribute(requestDoc.getRootElement(), "sourceNID", sourceNID);
				
			}
			else{//recording thumbnail get DMS details
						
				String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceResourceConst.RECORDINGTHUMBNAILPARAMETERS + SessionServiceConst.XQUERYXML;	    	   
				String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
				query = query.replace(SessionServiceResourceConst._FILENID_,sourceNID);				
				Document parametersDoc = GetObjectUtils.executeQuery(query,request, xmpp, log, db, serviceAgentFactory, resourceFactory);			
				String fileUUID = MRXMLUtils.getValueXpath(parametersDoc, "//fileUUID/UUID", "");
				MRXMLUtils.addAttribute(requestDoc.getRootElement(), "sourceNID", fileUUID);
				deviceJID = MRXMLUtils.getValueXpath(parametersDoc, "//contactJID", "");
				portId =  MRXMLUtils.getValueXpath(parametersDoc, "//dirID", "");			
			}	
			MRXMLUtils.addAttribute(requestDoc.getRootElement(), "portId", portId);		
						
			if(deviceJID.isEmpty()) {
				log.addInfo("ThumbnailStreamRequestHandler: Cannot Send " +requestName+ " since the device is offline ",MRLog.DEBUG,MRLog.NOTIFY);
				throw new Exception("Device is offline");
			}
			
			AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
			Element apiKeyTableEl = appServerAgent.getAPIKey();
			
			dstTitle = StringUtils.substringAfter(deviceJID, "@localhost/");
			if(dstTitle == null) {
				dstTitle =  "";
			} 				

			Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log, xmpp, db, serviceAgentFactory, request, apiKeyTableEl, dstTitle, request.getUserJID(),AuthScheme.EXPIRY.toString());

		    String msIPAddress =  MRXMLUtils.getAttributeValue(appResourceInfo, "ipAddress","");
			String publishURL = HttpAuthentication.generateThumbnailURL(sourceNID, type, HttpAuthentication.INFINITE_EXPIRY,msIPAddress,AuthScheme.EXPIRY.toString(), log,serviceAgentFactory, MRRequest.createRequest(deviceJID),db,xmpp,resourceFactory,""); 	// timestamp = INFINTE
			MRXMLUtils.addAttribute(requestDoc.getRootElement(), "publishURL", publishURL);
			MRXMLUtils.addAttribute(requestDoc.getRootElement(), "type", type);
			
			MRRequest agentRequest = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid());
			request.setRequestType(action + "PostingThumbnailRequest");
			
					
			log.addInfo("ThumbnailStreamRequestHandler: Sending " +requestName+ " to deviceJID = "+deviceJID ,MRLog.DEBUG,MRLog.NOTIFY);
			AppServerAgent.sendAgentRequestForUserRequest(agentRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
					deviceJID, requestName, MRConst.MEDIA_SERVICE, MRXMLUtils.documentToStringOmitDeclaration(requestDoc), this.getClass().getName());
			
			
		} catch (Exception e) {		
			e.printStackTrace();
			log.addInfo("Error in Thumbnail Request "+ action + "for " + sourceNID + e.toString() ,MRLog.ERROR,MRLog.NOTIFY);
		}
		
	}
	
	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		return "";
	}
	
}
