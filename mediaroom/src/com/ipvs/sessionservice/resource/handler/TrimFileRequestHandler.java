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
import com.ipvs.common.db.MRXMLDBUtils;
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
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.Utils;
import com.ipvs.utils.ValidationUtils;

@Path("Resource/File/Trim")
public class TrimFileRequestHandler extends FileBaseRequestHandler{

	@Context ServletContext servletContext; 
	@Context HttpServletRequest servletRequest;

   	@POST
    @Consumes({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
  	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML}) 
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers) throws MRException{
   		return new TrimFile().handleHttpRequest(xml, uriInfo, headers,servletRequest);
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
		Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(NID, request, log, db);
		Long playlength = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaFileDoc, "//PlayLength", "0"), 0);
		
		//only Media Clip allow with Ready State
		validateMedia(mediaFileDoc);

		//validate cutlist, Blank doesn't allow
		ValidationUtils.validateCutListOffset(cutList, playlength);
		

		String UUID = MRXMLUtils.getValueXpath(mediaFileDoc, "//UUID", "").trim();
		String dirNID = MRXMLUtils.getValueXpath(mediaFileDoc, "//ParentMediaDirNID", "").trim();
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaFileDoc, true, request, db, log);
		Element mediaStreamFileEl = MRXMLUtils.getElementXpath(mediaFileDoc.getRootElement(), "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
		Element fileStateEl =MRXMLUtils.getElementXpath(mediaFileDoc, "//StateFlag");
		String fileState = MRXMLUtils.getValue(fileStateEl);
		
		Element requestEl = MRXMLUtils.getElementXpath(requestDoc, "//TrimFile");
		MRXMLUtils.addAttribute(requestEl, MediaStoreServiceConst.UUID, UUID);
		MRXMLUtils.addAttribute(requestEl, "dirID", dirID);
		MRXMLUtils.addAttribute(requestEl, "dirNID", dirNID);
		MRXMLUtils.addAttribute(requestEl, "size", MRXMLUtils.getValueXpath(mediaFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, "0"));
		

		if(MediaStoreServiceConst.READY.equals(fileState)){
			MRXMLUtils.setValue(fileStateEl, "Busy");
			MRXMLUtils.setValueXpath(mediaFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, "0");
			MRXMLDBUtils.updateElement(request, log, db, NID, mediaFileDoc.getRootElement());
			sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, mediaStorePortElement, this.getClass().getName());
		}
		return getFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID), "","");
	}
	
	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
		
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.SUCCESS_STATE);
	    log.addInfo("TrimFile Agent Response State :" + state);
	    String mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(userRequestdoc, "//TrimFile", "byNID", "");
	    String size = MRXMLUtils.getAttributeValueXpath(userRequestdoc, "//TrimFile", "size", "");
        if (state == MediaStoreServiceConst.FAILURE_STATE) {
        	Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, userRequest, log, db);
        	MRXMLUtils.setValueXpath(mediaFileDoc, "//StateFlag", MediaStoreServiceConst.READY);
			MRXMLUtils.setValueXpath(mediaFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, size);
			MRXMLDBUtils.updateElement(userRequest, log, db, mediaStreamFileNID, mediaFileDoc.getRootElement());
            return null;
        }
        return getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileNID,"","");
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
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "Trimming of a media file contained within a media group is not allowed.");
		}
		
		if(!isWritable){
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "Request can not be accepted for non upgraded media.");
		}
     }
	
	private class TrimFile extends BaseHtppRequestHandler{
    	private static final String REST_SERVICE_NAME = "TrimFile";  
		public Response handleHttpRequest(String xml,UriInfo uriInfo, HttpHeaders headers,HttpServletRequest servletRequest) throws MRException{
	   		setInfo(servletContext); 
	   		super.servletRequest = servletRequest;
		   	
	   		Document responseDoc = null; 
	   		Document requestDoc =null; 
	   		//String NID ="";  
	   		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0); 
	   		if(acceptHeader == null) 
	   			acceptHeader = DEFAULT_MIME_SUFFIX; 
	   		
	   		try {
	   			validateUserAuthentication(headers);
			} catch (Exception validateuserExp) {
				log.addInfo("Trim File Failed! : Authorization " + validateuserExp.toString());
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(validateuserExp.toString()).build();
			}
	   		
	   		try { 
	   			String serviceName = SessionServiceUtils.getServiceName(REST_SERVICE_NAME+"Request", log); 
	   			if (acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
	   				xml = MRXMLUtils.jsonToXMLString(xml);
                }
	   			Element requestBodyEl = MRXMLUtils.stringToElement(xml);
              	requestDoc = getRequestDoc(uriInfo.getQueryParameters(), REST_SERVICE_NAME, serviceName,requestBodyEl,servletRequest.getHeader("X-Forwarded-Proto"));
              //	NID = MRXMLUtils.getAttributeValue(requestBodyEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");

	   		} catch (Exception requestParserExp) { 
	   			log.addInfo("TrimFileRequestHandler: Trim File Failed. Bad Request " + requestParserExp.toString()); 
	   			return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(requestParserExp.toString()).build(); 
	   		} 
	   		
	   		try { 
	   			MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid()); 
  		        responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	   			if(responseDoc != null) { 
	   				String responseStr = responseDoc.asXML();
	   				Element respFileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
	   				if(respFileEl == null ){
	   					return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(responseStr).build();
	   				}else{
		   				Element respEl = DocumentHelper.createElement("TrimFile");
		   				MRXMLUtils.addElement(respEl,(Element)respFileEl.clone());
		   				responseStr = respEl.asXML(); 
		   				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) { 
		   					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
		   				} 
			   			return Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build(); 
	   				}
	   			} 
				return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR ).type(acceptHeader).entity("File Trim Aborted!").build(); 
    
	   		} catch (Exception internalServerExp) { 
	   			log.addInfo("responseDoc: Trim File Failed. " + internalServerExp.toString()); 
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
