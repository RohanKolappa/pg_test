package com.ipvs.sessionservice.resource.handler;

import java.util.HashMap;
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
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.handler.AssetAdminUtils;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
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
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

@Path("Resource/File/PrepareForDownload")
public class PrepareFileRequestHandler extends FileBaseRequestHandler {
	
	@Context ServletContext servletContext; 

	@Context HttpServletRequest servletRequest;
  	@POST
    @Consumes({ MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML })
  	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML}) 
	public Response handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers) throws MRException{
   		return new Prepare().handleHttpRequest(xml, uriInfo, headers,servletRequest);
   	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		// get request document
		Document requestDoc = request.getMessage().getDocument();

		// create response document
		Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());
		// get element from request
		Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
		if (reqEL == null) {
			throw new Exception("Invalid request " + respRoot.getName());
		}

		String ownerJID = request.getUserJID();
		String extJID = MRXMLUtils.getAttributeValue(reqEL, "JID", "").trim();
		String mediaFormat = MRXMLUtils.getAttributeValue(reqEL, "format", "Proprietary").trim();
		String reqTimeStamp = MRXMLUtils.getAttributeValue(reqEL, "requestTimeStamp", "").trim();
		if (reqTimeStamp.equals("")) {
			reqTimeStamp = System.currentTimeMillis() + "";
		}
		if (extJID.equals("")) {
			extJID = ownerJID;
		}
		if(mediaFormat == null || mediaFormat.isEmpty()) {
			mediaFormat=FileSM.MEDIA_FORMAT_PROPRIETARY;
		}
		
		if(!mediaFormat.equals(FileSM.MEDIA_FORMAT_STANDARD) && !mediaFormat.equals(FileSM.MEDIA_FORMAT_PROPRIETARY) && !mediaFormat.equals(FileSM.MEDIA_FORMAT_BOTH)) {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid mediaFormat:=" + mediaFormat+". Supported Format "+FileSM.MEDIA_FORMAT_STANDARD+","+
											FileSM.MEDIA_FORMAT_PROPRIETARY+","+FileSM.MEDIA_FORMAT_BOTH);
		}
		
		PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.EXPORT_ROLE_ACTION,AMConst.MEDIA_STREAM_FILE_RESOURCE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
		MRConfig mrConfig = MRConfig.getInstance(objectType, log);
		String listName = mrConfig.getParentNIDKey(log);
		String parentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
		String NID = MRXMLUtils.getAttributeValue(reqEL, "byNID", "");
		String type = MRXMLUtils.getAttributeValue(reqEL, "type", "");
		String startOffset = MRXMLUtils.getAttributeValue(reqEL, "startOffset", "");
		String endOffset = MRXMLUtils.getAttributeValue(reqEL, "endOffset", "");
		String new_fileUUID = MRXMLUtils.getAttributeValue(reqEL, "new_fileUUID", "");

		String[] arrWhereXpath = { "@NID='" + NID + "'" };
		// read media file
		Element file_el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, null, arrWhereXpath, null, null, null, null);
		if (file_el == null) {
			throw new MRException(AMExceptionCodes.MEDIA_PREPARE_FILE_NOT_FOUND_EXCEPTION, "Can not Prepare. File not found ('" + NID + "') ");
		}

		String UUID = MRXMLUtils.getValueXpath(file_el, ".//UUID", "").trim();
		String dirNID = getPrepareFileDirectoryNID(request, file_el, NID, log, db);
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(MRXMLUtils.elementToDocument(file_el), true, request, db, log);
		Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(NID, request, log, db);
		String fileState = MRXMLUtils.getValueXpath(mediaFileDoc, "//StateFlag", "");
		String mediaType = MRXMLUtils.getValueXpath(mediaFileDoc, "//Type", "");
		String streamType = MRXMLUtils.getValueXpath(mediaFileDoc,"//StreamType", "");
	
		// This is a hack-fix for issue VNG# 3392 (playlength missing in .grp file)
		// send group playlength if the mediatype is mediagroup.
		String mediaGroupPlaylength = (MediaRoomServiceConst.MEDIAGROUP.equals(mediaType)) ? MRXMLUtils.getValueXpath(mediaFileDoc,"//MediaGroup/PlayLength", "0") : "";
	
		if (MediaRoomServiceConst.V2D.equals(streamType)) {
			// Cannot support Standard format for V2D media
			if(FileSM.MEDIA_FORMAT_STANDARD.equals(mediaFormat))
				throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid mediaFormat:=" + mediaFormat+". Proprietary media only supports '"+FileSM.MEDIA_FORMAT_PROPRIETARY+"' format");
			
			mediaFormat = FileSM.MEDIA_FORMAT_PROPRIETARY;
		}

		Document data = getPrepareFileRequestDoc(NID, dirNID, UUID, mediaFormat, startOffset, endOffset, reqTimeStamp, type, mediaGroupPlaylength, new_fileUUID, log);
		
		System.out.println("****** data request doc = " + data.asXML());
		if(!MediaStoreServiceConst.READY.equals(fileState) && !MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)){
			throw new MRException(AMExceptionCodes.VALID_ERROR_MEDIACLIP, "File in use.");
		}
		
		if(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType) && (startOffset.isEmpty() || endOffset.isEmpty())) {
			throw new MRException(AMExceptionCodes.VALID_ERROR_PVRMEDIACLIP, "Incorrect or Missing parameters for PVR Prepare");
		}
		
		if((!isFilePrepareInProgress(mediaFileDoc, mediaFormat, log)) || (!startOffset.isEmpty() && !endOffset.isEmpty()))
		{
			sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, data, mediaStorePortElement, this.getClass().getName());

			Element dwnldEl = MRXMLUtils.getElementXpath(mediaFileDoc,	"//Download");
			updateMediaFormatInfo(dwnldEl, mediaFormat, "Busy", log);
			MRXMLDBUtils.updateElement(request, log, db, NID, mediaFileDoc.getRootElement());
		}
		return getFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID, "","");
	}

	private String getPrepareFileDirectoryNID(MRRequest request, Element file_el, String NID, MRLog log, MRDB db) throws Exception {

		String queryFileName = SessionServiceConst.XQUERYPACKAGE + "ReadDirForPrepareFile" + SessionServiceConst.XQUERYXML;
		String dirQuery = FileUtils.readFileResource(queryFileName, request.getClass());
		dirQuery = dirQuery.replace("_PARENT_DIR_NID_", MRXMLUtils.getValueXpath(file_el, ".//ParentMediaDirNID", "").trim());
		dirQuery = dirQuery.replace("_CLIP_TYPE_", MRXMLUtils.getValueXpath(file_el, ".//Info/MediaStoreFileInfo/Type", "").trim());
		String responseXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, dirQuery, new HashMap<String, String>(), null);
		if (responseXML == null || responseXML.isEmpty()) {
			throw new MRException(AMExceptionCodes.MEDIA_PREPARE_PARENT_DIR_NOT_FOUND_EXCEPTION, "Can not Prepare. Directory not found ('" + NID + "') ");
		}
		Element elDir = MRXMLUtils.stringToElement(responseXML);
		if (elDir == null) {
			throw new MRException(AMExceptionCodes.MEDIA_PREPARE_PARENT_DIR_NOT_FOUND_EXCEPTION, "Can not Prepare. Directory not found for file ('" + NID + "') ");
		}
		// read dirNID from db resource.
		String dirNID = MRXMLUtils.getAttributeValue(elDir, "NID", "");
		return dirNID;
	}

	public Document getPrepareFileRequestDoc(String fileNID, String dirNID, String UUID, String mediaFormat, String startOffset, String endOffset, String requestTimeStamp,
			String type, String mediaGroupPlaylength, String new_fileUUID, MRLog log) throws Exception {
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document prepareRequestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(prepareRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, SessionServiceConst.PREPARE_FILE_DOWNLOAD_REQUEST);
		MRXMLUtils.setAttributeValueXPath(prepareRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(prepareRequestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(SessionServiceConst.PREPARE_FILE_DOWNLOAD_REQUEST + "Data"));
		Element requestEl = MRXMLUtils.newElement("PrepareFile");
		MRXMLUtils.addAttribute(requestEl, "requestTimeStamp", requestTimeStamp);
		Element fileEl = MRXMLUtils.addElement(requestEl, "File");
		MRXMLUtils.addAttribute(fileEl, "fileNID", fileNID);
		MRXMLUtils.addAttribute(fileEl, "format", mediaFormat);
		MRXMLUtils.addAttribute(fileEl, "startOffset", startOffset);
		MRXMLUtils.addAttribute(fileEl, "endOffset", endOffset);
		MRXMLUtils.addAttribute(fileEl, "dirNID", dirNID);
		MRXMLUtils.addAttribute(fileEl, "UUID", UUID);
		MRXMLUtils.addAttribute(fileEl, "type", type);
		MRXMLUtils.addAttribute(fileEl, "mediaGroupPlaylength", mediaGroupPlaylength);
		MRXMLUtils.addAttribute(fileEl, "new_fileUUID", new_fileUUID);
		

		MRXMLUtils.addElement(requestDataEl, requestEl);
		return prepareRequestDoc;
	}
	
	private boolean isFilePrepareInProgress(Document mediaFileDoc, String format, MRLog log) throws Exception{    	
    	log.addInfo("PrepareFileRequestHandler:isFilePrepared: format::" + format);
    	if ("Standard".equalsIgnoreCase(format)) {
    		return fileFormatCheck(mediaFileDoc, "Standard", log);  
    	}else if ("Proprietary".equalsIgnoreCase(format)) {
    		return fileFormatCheck(mediaFileDoc, "Proprietary", log);  
    	}else {
    		if(!fileFormatCheck(mediaFileDoc, "Standard", log) || !fileFormatCheck(mediaFileDoc, "Proprietary", log)){
    			return false;
    		}
    		return true;
    	}
    	
    }

	private boolean fileFormatCheck(Document mediaFileDoc, String mediaFormat, MRLog log) throws Exception {
		Element mediaElement =MRXMLUtils.getElementXpath(mediaFileDoc, "//Media[@format='"+mediaFormat+"']");		
		String stateFlag = MRXMLUtils.getAttributeValue(mediaElement, "state", "");
		if ((!stateFlag.equalsIgnoreCase("Busy"))) {
            return false;
        } else {
            return true;
        }
	}
	
	public void updateMediaFormatInfo(Element downloadEl, String mediaFormat, String state, MRLog log) throws Exception {
		if (downloadEl == null || mediaFormat.isEmpty()) {
			return;
		}
		if (mediaFormat.equals("Proprietary") || mediaFormat.equals("Standard")) {
            updateMediaEl(downloadEl, mediaFormat, state, log);
        } else {
			updateMediaEl(downloadEl, "Proprietary", state, log);
			updateMediaEl(downloadEl, "Standard", state, log);
		}
    }
	
	private void updateMediaEl(Element downloadEl, String mediaFormat, String state, MRLog log) throws Exception {
    	if(downloadEl == null || mediaFormat.isEmpty()){
    		return;
    	}
		Element mediaEl = MRXMLUtils.getElementXpath(downloadEl, "//Media[@format='" + mediaFormat + "']");
		if (mediaEl != null) {
			MRXMLUtils.setAttributeValue(mediaEl, "format", mediaFormat);
			MRXMLUtils.setAttributeValue(mediaEl, "state", state);
		} else {
			Element mediaNewEl = DocumentHelper.createElement("Media");
			MRXMLUtils.setAttributeValue(mediaNewEl, "format", mediaFormat);
			MRXMLUtils.setAttributeValue(mediaNewEl, "state", state);
			MRXMLUtils.setAttributeValue(mediaNewEl, "stale", "");
			MRXMLUtils.addElement(downloadEl, (Element) mediaNewEl.clone());
		}
	}
	
	   @Override
	    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc,
	            MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {

	        log.addInfo("PrepareFileRequestHandler handleAgentResponse");
	        Element prepareElement = MRXMLUtils.getElementXpath(userRequest.getMessage().getDocument(), "//PrepareFile");
	        String fileNID = MRXMLUtils.getAttributeValue(prepareElement, "byNID", "");
	        String typeOfRequest = MRXMLUtils.getAttributeValue(prepareElement, "type", "");
	        boolean callNext = false;
	        String transition = null;
	        Element queueEL = null;	       
	        String error = null;
	        int errorCode = -1;
	        String[] arrWhereXpath = { "//Internal/@fileNID='" + fileNID + "' and @queue_state > -1 and @state != 'completed' " };
	        Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.SUCCESS_STATE);
	        if (state == MediaStoreServiceConst.FAILURE_STATE) {
	        	 Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(fileNID, userRequest, log, db);	              
	             Element dwnldEl = MRXMLUtils.getElementXpath(mediaFileDoc,	"//Download");             
	             List<?> prepareFileList =MRXMLUtils.getListXpath(userRequestdoc, "//PrepareFile");
	             for(Object o:prepareFileList) {
	            	 Element prepareFileElement = (Element)o;
	            	 
			             String format = MRXMLUtils.getAttributeValueXpath(prepareFileElement, "format", "Proprietary").trim();
			             String mediaState = MRXMLUtils.getAttributeValueXpath(dwnldEl, "./Media[@format='"+format+"']", "state", "");
			             
			             if(MediaStoreServiceConst.BUSY.equals(mediaState)){
			            			 updateMediaFormatInfo(dwnldEl, format, SessionServiceConst.NOT_READY, log);
			            			 
	            	 }
	             }
	             MRXMLDBUtils.updateElement(userRequest, log, db, fileNID, mediaFileDoc.getRootElement());
	        }
	        // db read
	        queueEL = FileSMUtils.readQueueElement_db_whereXpath(userRequest, db, log, arrWhereXpath);
	        if (queueEL != null) {
	            log.addInfo("state :::" + state);
	                        
	            if (state == MediaStoreServiceConst.FAILURE_STATE) {
	                error = MRXMLUtils.getValueXpath(agentResponseDoc, "//Description", "AgentResponseFailed");
	                if (error.equals("")) {
	                    String action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim();
	                    error = "Error during PrepareFileRequestHandler::" + FileSMUtils.getActionDescription(action, log) + "'";
	                }
	                errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
	               
	            }

	            String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim();

	            if (typeOfRequest.equalsIgnoreCase(FileSM.TYPE_EXPORT)) {
	                if (state == MediaStoreServiceConst.FAILURE_STATE) {
	                    transition = FileSMUtils.getTransitionError(FileSM.ACTION_EXPORT_PACK, log);
	                }
	            } else if (typeOfRequest.equalsIgnoreCase(FileSM.TYPE_OFFLOAD)) {
	                if (state == MediaStoreServiceConst.FAILURE_STATE) {
	                    transition = FileSMUtils.getTransitionError(FileSM.ACTION_OFFLOAD_PACK, log);
	                }
	            }
	            if (transition != null) {
	                FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, null, queueState,
	                        this.getClass().getName());
	                try {
	                    if (error != null) {
	                        stateMachine.setErrorCode(errorCode);
	                        stateMachine.setError(error);
	                    }
	                    // call handle transition
	                    stateMachine.handleTransition(transition);
	                    String sm_state = stateMachine.getState();
	                    callNext = FileSM.STATE_COMPLETED.equals(sm_state) || stateMachine.getError() != null;
	                    if (callNext) {
	                        log.addInfo("FileSM:PrepareFileRequestHandlerhandleAgentResponset: callNext ??" + sm_state + " ERROR='"
	                                + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) + "'");
	                    }
	                } finally {
	                    FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
	                }
	            }
	        }

	        return null;
	    }
	private class Prepare extends BaseHtppRequestHandler{

		private static final String SERVICE_NAME = "PrepareFile";  
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
			} catch (Exception e1) {
				log.addInfo("Prepare File Failed! : Authorization " + e1.toString());
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).type(MediaType.TEXT_XML).entity(e1.toString()).build();
			}
	   		try { 
	   			String serviceName = SessionServiceUtils.getServiceName(SERVICE_NAME+"Request", log); 
	   			
	   			if (acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX)) {
	   				xml = MRXMLUtils.jsonToXMLString(xml);
                }
	   			
	   			//Change request Element from PrepareFileForDownload to Prepare File
                Element requestBodyEl = MRXMLUtils.stringToElement(xml);
                Element prepareFileEl = DocumentHelper.createElement(SERVICE_NAME);
                MRXMLUtils.addAttribute(prepareFileEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, MRXMLUtils.getAttributeValue(requestBodyEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE));
                MRXMLUtils.addAttribute(prepareFileEl, "format", MRXMLUtils.getAttributeValue(requestBodyEl, "format"));
                
              	requestDoc = getRequestDoc(uriInfo.getQueryParameters(), SERVICE_NAME, serviceName,prepareFileEl,servletRequest.getHeader("X-Forwarded-Proto")); 
	   		} catch (Exception e) { 
	   			e.printStackTrace();
	   			log.addInfo("PrepareFileRequestHandler: PrepareFile Failed. Bad Request " + e.toString()); 
	   			return Response.status(HttpServletResponse.SC_BAD_REQUEST).type(acceptHeader).entity(e.toString()).build(); 
	   		} 
	   		try { 
	   			MRRequest request = MRRequest.createRequest(userJID, requestDoc, xmpp.getXMPPUserJid()); 
  		        responseDoc = getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	   			if(responseDoc != null) { 
	   				Element respFileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
	   				//changing file response name from PrepareFile to PrepareFileForDownload
	   				Element respEl = DocumentHelper.createElement("PrepareFileForDownload");
	   				MRXMLUtils.addElement(respEl,(Element)respFileEl.clone());
	   				String responseStr = respEl.asXML(); 
	   				if(acceptHeader.endsWith(JSON_MIME_SUFFIX) || acceptHeader.endsWith(DEFAULT_MIME_SUFFIX) ) { 
	   					responseStr = MRXMLUtils.xmlToJSONString(responseStr, JSON_INDENT);
	   				} 
	   			Response response =  Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity(responseStr).build(); 
	   			return response; 
	   			} 
				return Response.status(HttpServletResponse.SC_OK).type(acceptHeader).entity("Prepare File Request Failed !").build(); 
    
	   		} catch (Exception e) { 
	   			log.addInfo("PrepareFileRequestHandler: Prepare File Failed. " + e.toString()); 
	   			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).type(acceptHeader).entity(e.toString()).build(); 
	   		} 
		}
		@Override
		protected String getResponseString(Document responseDoc, String requestName) throws Exception {
			// TODO Auto-generated method stub
			return null;
		}
	}

}
