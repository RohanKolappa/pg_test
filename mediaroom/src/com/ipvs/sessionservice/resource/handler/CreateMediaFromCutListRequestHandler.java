package com.ipvs.sessionservice.resource.handler;

import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.handler.AssetAdminUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.servlet.BaseHtppRequestHandler;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.Utils;
import com.ipvs.utils.ValidationUtils;

@Path("Resource/File/CreateMediaFromCutList")
public class CreateMediaFromCutListRequestHandler extends FileBaseRequestHandler{

	@Context ServletContext servletContext; 
	@Context HttpServletRequest servletRequest;
   	@POST
    @Consumes({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
  	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML}) 
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers) throws MRException{
   		return new CreateMediaCutList().handleHttpRequest(xml, uriInfo, headers, servletRequest);
   	}
   	
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document requestDoc = request.getMessage().getDocument();
		Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());
		Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
		if (reqEL == null) {
			throw new Exception("Invalid request " + respRoot.getName());
		}
		String NID = MRXMLUtils.getAttributeValue(reqEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");
		String cutList = MRXMLUtils.getAttributeValue(reqEL, "cutList", "").trim();
		String title = MRXMLUtils.getAttributeValue(reqEL, "title", "");
		Element metaDataEl = MRXMLUtils.getElementXpath(reqEL, "//MetaData");
		Element descriptionEl = MRXMLUtils.getElementXpath(reqEL, "//Description");
		Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(NID, request, log, db);
		Long playlength = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaFileDoc, "//PlayLength", "0"), 0);
		
		if (descriptionEl == null) {
		     throw new Exception("Invalid request " + respRoot.getName());
		 }

		//validate media
		validateMedia(mediaFileDoc);
		
		if(!cutList.isEmpty())
		{
			ValidationUtils.validateCutListOffset(cutList, playlength);		
		}
		
		String UUID = MRXMLUtils.getValueXpath(mediaFileDoc, "//UUID", "").trim();
		String dirNID = MRXMLUtils.getValueXpath(mediaFileDoc, "//ParentMediaDirNID", "").trim();
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaFileDoc, true, request, db, log);
		
		Document mediaStreamFileResourceDoc = addMediaStreamFileInDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaFileDoc, title, metaDataEl, descriptionEl, dirNID);
		Element mediaStreamFileEl = MRXMLUtils.getElementXpath(mediaStreamFileResourceDoc.getRootElement(), "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
		
		MRXMLUtils.setAttributeValue(reqEL, "new_fileNID", MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID, ""));
		metaDataEl = (metaDataEl == null) ? MRXMLUtils.getElementXpath(mediaStreamFileEl, "//MetaData") : metaDataEl;
		
		Document data = getRequestDocument(mediaStreamFileResourceDoc, metaDataEl, NID, dirID, cutList, title, UUID, dirNID, "CreateMediaFromCutListRequest", descriptionEl, log);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, data, mediaStorePortElement, this.getClass().getName());
		
		return getFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID), "","");
	}
	
	private void validateMedia(Document mediaFileDoc) throws Exception{
		Element fielTypeEl =MRXMLUtils.getElementXpath(mediaFileDoc, "//Type");
		String mediaType = MRXMLUtils.getValue(fielTypeEl);
		Element fileStateEl =MRXMLUtils.getElementXpath(mediaFileDoc, "//StateFlag");
		String fileState = MRXMLUtils.getValue(fileStateEl);
		String mediagroupID = MRXMLUtils.getValueXpath(mediaFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
		Boolean isWritable = Boolean.parseBoolean(MRXMLUtils.getValueXpath(mediaFileDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE));
				
		if(!mediaType.isEmpty() && !MediaRoomServiceConst.MEDIACLIP.equals(mediaType)){
      		throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, mediaType+" Not supported.");
      	}
      	
		if(!MediaStoreServiceConst.READY.equals(fileState)){
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "File in use.");
		}
		
		if(!"".equals(mediagroupID)){
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "Cutlist of a media file contained within a media group is not allowed.");
		}
		
		if(!isWritable){
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "Request can not be accepted for non upgraded media.");
		}
     }
	
	private Document addMediaStreamFileInDB(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document mediaFileDoc, String title, Element metaData, Element descriptionEl, String parentDirNID) throws Exception{
		
		Document mediaStreamFileDoc = null;
	    String new_UUID = Utils.getUUID(); // generate a new UUID
	    Element origMediaStreamFileEl = MRXMLUtils.getElementXpath(mediaFileDoc.getRootElement(), "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
	    Element groupsEl = MRXMLUtils.getElementXpath(origMediaStreamFileEl, "//Groups");
	    metaData = (metaData == null) ? MRXMLUtils.getElementXpath(origMediaStreamFileEl, "//MetaData") : metaData;
	    long trimlength = 0;
	  
	    mediaStreamFileDoc = SessionResourceUtils.getMediaStreamFileDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.MEDIA_CLIP, trimlength, title, descriptionEl.getTextTrim(), parentDirNID, System.currentTimeMillis(), "", "", groupsEl, metaData, "", "", new_UUID);
	
	    request.setSuperUser(true);
		mediaStreamFileDoc = AddObjectUtils.addObjectInDatabaseNoValidationCheck(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileDoc, AMConst.MediaStreamFileResource);
	    // Subscribe for file updates.
		MRXMLUtils.setAttributeValue(mediaStreamFileDoc.getRootElement(), SessionServiceConst.SUBSCRIPTION_ID, new_UUID);
		MRXMLUtils.setAttributeValue(mediaStreamFileDoc.getRootElement(), SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_EXTENDEDURL);
		GetObjectUtils.subscribeUser(mediaStreamFileDoc.getRootElement(), mediaStreamFileDoc, SessionServiceConst.MEDIA_STREAM_FILE_RESOURCE, request, serviceAgentFactory);
		
		return mediaStreamFileDoc;
		
	}
	
	private Document getRequestDocument(Document mediaStreamFileDoc, Element metaDataEl, String fileNID, String dirID,String cutList, String title, String UUID, String dirNID, String requestName, Element descriptionEl, MRLog log) throws Exception {

		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
		Element mediaStreamFileEl = MRXMLUtils.getElementXpath(mediaStreamFileDoc.getRootElement(), "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
		
		MRXMLUtils.addAttribute(requestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, fileNID);
		MRXMLUtils.addAttribute(requestEl, "dirID", dirID);
		MRXMLUtils.addAttribute(requestEl, "cutList", cutList);
		MRXMLUtils.addAttribute(requestEl, "title", title);
		MRXMLUtils.addAttribute(requestEl, "UUID", UUID);
		MRXMLUtils.addAttribute(requestEl, "dirNID", dirNID);
		MRXMLUtils.addAttribute(requestEl, "new_fileNID", MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID, ""));
		MRXMLUtils.addAttribute(requestEl, "new_UUID", MRXMLUtils.getValueXpath(mediaStreamFileEl, "//UUID", ""));
		if(metaDataEl != null) {
            MRXMLUtils.addElement(requestEl, (Element)metaDataEl.clone());
        }
		if(descriptionEl != null) {
            MRXMLUtils.addElement(requestEl, (Element)descriptionEl.clone());
        }
		
		MRXMLUtils.addElement(requestDataEl, requestEl);
		return requestDoc;
	}
	
	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
		
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.SUCCESS_STATE);
	    log.addInfo("CreateMediaFromCutList Agent Response State :" + state);
	    String mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(userRequestdoc, "//CreateMediaFromCutList", "new_fileNID", "");
        String error = null;
        int errorCode = -1;
        // Mark the file in Error state in DB, if response state is failure.
        if (state == MediaStoreServiceConst.FAILURE_STATE) {
            error = MRXMLUtils.getValueXpath(agentResponseDoc, "//Description", "AgentResponseFailed");
            errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
            if (error.equals("")) {
                error = "Error during CreateMediaFromCutList! ErrorCode:" + errorCode;
            }
            String userJID = MRXMLUtils.getAttributeValueXpath(userRequestdoc, MRConst.HEADER_XPATH, MRConst.USER_JID, "");
            
            MediaStoreServiceUtils.updateMediaStreamFileState(mediaStreamFileNID, MediaStoreServiceConst.ERROR, "", userJID, xmpp, log, db);
        }
        return getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileNID,"","");
	}
	
	private class CreateMediaCutList extends BaseHtppRequestHandler{

		private static final String REST_SERVICE_NAME = "CreateMediaFromCutList";  
		public Response handleHttpRequest(String xml,UriInfo uriInfo, HttpHeaders headers,HttpServletRequest servletRequest) throws MRException{
	   		setInfo(servletContext); 
	   		super.servletRequest = servletRequest;		   	
	   		Document responseDoc = null; 
	   		Document requestDoc =null; 
	   		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0); 
	   		if(acceptHeader == null) {
                acceptHeader = DEFAULT_MIME_SUFFIX;
            } 
	   		
	   		try {
	   			validateUserAuthentication(headers);
			} catch (Exception validateuserExp) {
				log.addInfo("Create Media From cutlist Failed! : Authorization " + validateuserExp.toString());
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(validateuserExp.toString()).build();
			}
	   		
	   		try { 
	   			String serviceName = SessionServiceUtils.getServiceName(REST_SERVICE_NAME+"Request", log); 
	   			if (acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
	   				xml = MRXMLUtils.jsonToXMLString(xml);
                }
	   			Element requestBodyEl = MRXMLUtils.stringToElement(xml);
              	requestDoc = getRequestDoc(uriInfo.getQueryParameters(), REST_SERVICE_NAME, serviceName,requestBodyEl,servletRequest.getHeader("X-Forwarded-Proto"));

	   		} catch (Exception requestParserExp) { 
	   			log.addInfo("CreateMediaFromCutListRequestHandler: Create Media From cutlist Failed. Bad Request " + requestParserExp.toString()); 
	   			return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(requestParserExp.toString()).build(); 
	   		} 
	   		
	   		try { 
	   			MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid()); 

  		        responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	   			if(responseDoc != null) { 
	   				Element respFileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
	   				Element respEl = DocumentHelper.createElement("CreateMediaFromCutList");
	   				MRXMLUtils.addElement(respEl,(Element)respFileEl.clone());
	   				String responseStr = respEl.asXML(); 
	   				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) { 
	   					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
	   				} 
	   			Response response =  Response.status(HttpServletResponse.SC_CREATED).type(acceptHeader).entity(responseStr).build(); 
	   			return response; 
	   			} 
				return Response.status(HttpServletResponse.SC_CREATED ).type(acceptHeader).entity("Media creation started From cutlist !").build(); 
    
	   		} catch (Exception internalServerExp) { 
	   			log.addInfo("responseDoc: Create Media From CutList Failed. " + internalServerExp.toString()); 
	   			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(internalServerExp.toString()).build(); 
	   		} 
		}
		@Override
		protected String getResponseString(Document responseDoc, String requestName) throws Exception {
			// TODO Auto-generated method stub
			return null;
		}
	}
}
