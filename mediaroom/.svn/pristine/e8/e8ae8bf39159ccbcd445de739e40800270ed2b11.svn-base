package com.ipvs.sessionservice.resource.handler;

import java.util.List;

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
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

@Path("Resource/File/ClearDownloadCache")
public class ClearDownloadCacheRequestHandler extends FileBaseRequestHandler{

	@Context ServletContext servletContext; 
	@Context HttpServletRequest servletRequest;

   	@POST
    @Consumes({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
  	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML}) 
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers) throws MRException{
   		return new ClearDownloadCache().handleHttpRequest(xml, uriInfo, headers, servletRequest);
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
		
		Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(NID, request, log, db);
		String UUID = MRXMLUtils.getValueXpath(mediaFileDoc, "//UUID", "").trim();
		String dirNID = MRXMLUtils.getValueXpath(mediaFileDoc, "//ParentMediaDirNID", "").trim();
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaFileDoc, true, request, db, log);
		String mediaType = MRXMLUtils.getValueXpath(mediaFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		Document data = null;
		
		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			String mediaGroupID = MRXMLUtils.getValueXpath(mediaFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
			List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
            //clear Cache for all files exist in group 
			if ((listOfFiles != null) && (!listOfFiles.isEmpty())) {
                for (Object object:listOfFiles ) { 
					Element fileEL = (Element)object;
					String fileNID= MRXMLUtils.getAttributeValue(fileEL, AMConst.NID, "");
					String fileUUID= MRXMLUtils.getValueXpath(fileEL, ".//UUID", "").trim();
					Element filedownloadEl = MRXMLUtils.getElementXpath(fileEL,"//Download");
					if(!isClearCacheInProgress(filedownloadEl, log)){
						data = getRequestDocument(fileNID, dirID, fileUUID, dirNID, "ClearDownloadCacheRequest", log);
						sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, data, mediaStorePortElement, this.getClass().getName());
						List<?> mediaElList = MRXMLUtils.getListXpath(filedownloadEl, "/*[@NID='"+fileNID+"']//Media");
						for (Object obj : mediaElList) {
							Element mediaEL = (Element) obj;
							MRXMLUtils.setAttributeValue(mediaEL, "state", "Busy");
						}
						MRXMLDBUtils.updateElement(request, log, db, fileNID, fileEL);
					}

				}
            }
		}
		
		Element downloadEl = MRXMLUtils.getElementXpath(mediaFileDoc,"//Download");
		if(!isClearCacheInProgress(downloadEl, log))
		{
			data=getRequestDocument(NID, dirID, UUID, dirNID, "ClearDownloadCacheRequest", log);
			sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, data, mediaStorePortElement, this.getClass().getName());
			List<?> mediaElList = MRXMLUtils.getListXpath(downloadEl, "//Media");
			for (Object obj : mediaElList) {
				Element mediaEL = (Element) obj;
				MRXMLUtils.setAttributeValue(mediaEL, "state", "Busy");
			}
			MRXMLDBUtils.updateElement(request, log, db, NID, mediaFileDoc.getRootElement());
		}
		return getFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID, "","");
	}
	
	private Document getRequestDocument(String fileNID, String dirID, String UUID, String dirNID, String requestName, MRLog log) throws Exception {

		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
		MRXMLUtils.addAttribute(requestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, fileNID);
		MRXMLUtils.addAttribute(requestEl, "dirID", dirID);
		MRXMLUtils.addAttribute(requestEl, "UUID", UUID);
		MRXMLUtils.addAttribute(requestEl, "dirNID", dirNID);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		return requestDoc;
	}
	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
		return null;
	}
	
	private boolean isClearCacheInProgress(Element downloadEl, MRLog log) throws Exception {
		boolean isclearCache = false;
		Element mediaEmptyEl = MRXMLUtils.getElementXpath(downloadEl,"//Media[@state='Prepared']");
		
		if(mediaEmptyEl!=null){
			isclearCache = false;
		}else{
			isclearCache = true;
		}
		
		return isclearCache;
  }
	
	private boolean isFilePrepared(Element downloadEl, MRLog log) throws Exception {
		boolean isclearCache = false;
		List<?> mediaElList = MRXMLUtils.getListXpath(downloadEl, "//Media");
		for (Object obj : mediaElList) {
			Element mediaEL = (Element) obj;
			String state = mediaEL.attributeValue("state");
			if (state.isEmpty() || !state.equals("Prepared")) {
				isclearCache = true;
				break;
			} else {
				isclearCache = false;
			}
		}
		return isclearCache;
  }
	
	private class ClearDownloadCache extends BaseHtppRequestHandler{

		private static final String REST_SERVICE_NAME = "ClearDownloadCache";  
		public Response handleHttpRequest(String xml,UriInfo uriInfo, HttpHeaders headers,HttpServletRequest servletRequest) throws MRException{
	   		setInfo(servletContext);
	   		super.servletRequest = servletRequest;
	   		Document requestDoc =null; 
	   		Document responseDoc = null; 
	   		String NID ="";  
	   		String acceptHeader = headers.getRequestHeader(HttpHeaders.ACCEPT).get(0); 
	   		if(acceptHeader == null) 
	   			acceptHeader = DEFAULT_MIME_SUFFIX; 
	   		
	   		try {
	   			validateUserAuthentication(headers);
			} catch (Exception validateuserExp) {
				log.addInfo("Clear Download Cache Failed! : Authorization " + validateuserExp.toString());
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(validateuserExp.toString()).build();
			}
	   		
	   		try { 
	   			String serviceName = SessionServiceUtils.getServiceName(REST_SERVICE_NAME+"Request", log); 
	   			if (acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
	   				xml = MRXMLUtils.jsonToXMLString(xml);
                }
	   			Element requestBodyEl = MRXMLUtils.stringToElement(xml);
              	requestDoc = getRequestDoc(uriInfo.getQueryParameters(), REST_SERVICE_NAME, serviceName,requestBodyEl,servletRequest.getHeader("X-Forwarded-Proto"));
              	NID = MRXMLUtils.getAttributeValue(requestBodyEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");

	   		} catch (Exception requestParserExp) { 
	   			log.addInfo("ClearDownloadChacheRequestHandler: Clear Download Cache Failed. Bad Request " + requestParserExp.toString()); 
	   			return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(requestParserExp.toString()).build(); 
	   		} 
	   		
	   		try { 
	   			MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid()); 
	   			Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(NID, request, log, db);
	   			Element downloadEl = MRXMLUtils.getElementXpath(mediaFileDoc,"//Download");
	   			if(!isFilePrepared(downloadEl, log)){
	   				responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	   				if(responseDoc != null) { 
		   				Element respFileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
		   				Element respEl = DocumentHelper.createElement("ClearDownloadCache");
		   				MRXMLUtils.addElement(respEl,(Element)respFileEl.clone());
		   				String responseStr = respEl.asXML(); 
		   				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) { 
		   					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
		   				} 
		   				Response response =  Response.status(HttpServletResponse.SC_ACCEPTED).type(acceptHeader).entity(responseStr).build(); 
		   				return response; 
		   			}
	   				else 
	   					return Response.status(HttpServletResponse.SC_ACCEPTED).type(acceptHeader).entity("Clear Download Cache request accepted.").build(); 
	   			}else{
	   				return Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity("File not cached, nothing to clean.").build();
	   			}
	   		} catch (Exception internalServerExp) { 
	   			log.addInfo("responseDoc: Clear Download Cache Failed. " + internalServerExp.toString()); 
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
