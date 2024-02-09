package com.ipvs.assetadminservice.file.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.resource.handler.ClearDownloadCacheRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public class PublishPrepareFileStatusRequestHandler extends BasePublishFileRequestHandler implements MRRequestHandlerI {

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
		Document requestDocument = request.getMessage().getDocument();
		List<?> listOfFiles  =  MRXMLUtils.getListXpath(requestDocument, "//File");
		Document resourceDoc = getData(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDocument);
		Document responseDoc = request.createResponseData();
		
		for(Object object:listOfFiles) {
			Element fileEl = (Element) object;
			String mediaStoreFileNID = MRXMLUtils.getAttributeValue(fileEl,SessionServiceConst.ID,"");
			Element mediaStreamFileEl = MRXMLUtils.getElementXpath(resourceDoc, "//MediaStreamFileResource[.//UUID='"+mediaStoreFileNID+"']");
			String fileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID);
			String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
			Element downloadElement= MRXMLUtils.getElement(fileEl, "Download");
			String typeOfRequest = MRXMLUtils.getAttributeValue(fileEl, "typeOfRequest","");
			
			if(downloadElement != null) {
				MediaStoreServiceUtils.updateDownloadInfo(mediaStreamFileEl, downloadElement);
				Element emptyEl = MRXMLUtils.getElementXpath(MRXMLUtils.getElementXpath(mediaStreamFileEl,"//Download"),"//Media[@format='']");
				if(emptyEl != null) {
                    emptyEl.detach();
                }

				request.setSuperUser(true);
				// Update download state in the database.
				UpdateObjectUtils.updateElementNoValidationCheck(request, xmpp, log, db, AMConst.MediaStreamFileResource, fileNID, MRXMLUtils.elementToDocument(mediaStreamFileEl), "");
				moveToNextTransition(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileEl, fileNID, downloadElement);
				sendFileNotification(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileNID, SessionServiceConst.UPDATE, mediaType);
			}
			if(typeOfRequest.equalsIgnoreCase("ClearCache")){
					// Update download Element
					Element downloadEl = MRXMLUtils.getElementXpath(mediaStreamFileEl,"//Download");
					Element newDownloadEl = createMediaDownloadElement();
					MRXMLUtils.replaceContent(downloadEl, newDownloadEl);
					
					//update size
			        long sizeSent = Utils.getLongValue(MRXMLUtils.getAttributeValue(fileEl, SessionServiceConst.SIZE,"0"),0);
			           if(sizeSent!=0) {
			               MRXMLUtils.setValueXpath(mediaStreamFileEl, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(sizeSent));
			            } 
				
				
				request.setSuperUser(true);
				// Update download state & size in the database.
				UpdateObjectUtils.updateElementNoValidationCheck(request, xmpp, log, db, AMConst.MediaStreamFileResource, fileNID, MRXMLUtils.elementToDocument(mediaStreamFileEl), "");
				sendFileNotification(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileNID, SessionServiceConst.UPDATE, mediaType);
				
				
				String mediaGroupID = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(mediaStreamFileEl), "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
				
				if(!mediaGroupID.isEmpty()){
					String[] whereXpath = {
			                "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "[(" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + mediaGroupID + "') " + " and (" +
			                MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "!='" + MediaRoomServiceConst.MEDIAGROUP + "')]"
			            };
					Document fileListDoc = MediaStoreServiceUtils.getMediaStreamFileResourceDoc(whereXpath, request.getUserJID(), log, db);
					
					List<?> mediaElList = MRXMLUtils.getListXpath(fileListDoc, "//Media[@state='Prepared']");
					if(mediaElList ==null || mediaElList.isEmpty()){
						Element groupListEl= MRXMLUtils.getElementXpath(resourceDoc, "//GroupList/"+ AMConst.MEDIA_STREAM_FILE_RESOURCE + "[(//GroupList/"+ AMConst.MEDIA_STREAM_FILE_RESOURCE+ "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT+"/" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + mediaGroupID + "')]");
						String mediaGroupFileNID = MRXMLUtils.getAttributeValue(groupListEl , AMConst.NID);
						clearGroupFileCache(request, xmpp, db, mediaGroupFileNID, "ClearDownloadCacheRequest", log, resourceFactory, serviceAgentFactory);
					}
				}
		     }
			Element responseFileEl = MRXMLUtils.newElement("File");
			MRXMLUtils.addAttribute(responseFileEl, SessionServiceConst.NID, MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID, ""));
			MRXMLUtils.addAttribute(responseFileEl, SessionServiceConst.ID, MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//UUID", ""));
			MRXMLUtils.addElement(responseDoc.getRootElement(), (Element)responseFileEl.clone());
		}
		return responseDoc;
	}
	
	private void moveToNextTransition(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory, Element fileEl,
			String fileNID, Element downloadElement) throws Exception {

		String error = null;
		int errorCode = -1;
		String typeOfRequest = MRXMLUtils.getAttributeValue(fileEl, "typeOfRequest", "");
		String prepareFileName = MRXMLUtils.getAttributeValue(fileEl, "prepareFileName", "");
		String resp_state = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(downloadElement, "Media"), "state");
		log.addInfo("PublishPrepareFileRequestHandler Type Of Request From PrepareFileForDownloadRequestHandler::" + typeOfRequest);
		boolean callNext = false;
		  try {
		String[] arrWhereXpath = { "//Internal/@fileNID='" + fileNID + "' and @queue_state > -1 and @state != 'completed' " };
		// do db read
		Element queueEL = FileSMUtils.readQueueElement_db_whereXpath(request, db, log, arrWhereXpath);
		if (queueEL == null) {
			log.addInfo("FileSM:handleAgentResponse ABORT:requestName := PublishPrepareFileRequestHandler from PrepareFileForDownloadRequestHandler " + " ABORTING queueEL is NULL ",
					MRLog.INFO, MRLog.NOTIFY);
			callNext = true;
		}else{
		if (resp_state.equalsIgnoreCase("Error")) {
			error = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Description", "");
			if (error.equals("")) {
				String action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim();
				error = "Error during PrepareFileRequestHandler::" + FileSMUtils.getActionDescription(action, log) + "'";
			}
			errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Code", ""), AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION);
		}
		String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim();
		Element external = queueEL.element("External");
		MRXMLUtils.addAttribute(external, "fileName", prepareFileName);
		// TODO figure out type
		String transition = null;
		if (typeOfRequest.equalsIgnoreCase(FileSM.TYPE_EXPORT)) {
			if (resp_state.equalsIgnoreCase("Error")) {
				transition = FileSMUtils.getTransitionError(FileSM.ACTION_EXPORT_PACK, log);
			} else if (resp_state.equalsIgnoreCase("Prepared")) {
				transition = FileSMUtils.getTransitionOK(FileSM.ACTION_EXPORT_PACK, log);
			}
		} else if (typeOfRequest.equalsIgnoreCase(FileSM.TYPE_OFFLOAD)) {
			if (resp_state.equalsIgnoreCase("Error")) {
        		transition = FileSMUtils.getTransitionError(FileSM.ACTION_OFFLOAD_PACK, log);
            } else {
            	 transition = FileSMUtils.getTransitionOK(FileSM.ACTION_OFFLOAD_PACK, log);
            }
        }
		if (transition != null) {
			 if (transition.equals(FileSM.DMSSTATUS_TRANS_OK)) {
				 queueState = FileSM.STATE_DMSSTATUS;
             }
			FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, profileFactory, null, queueState,
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
					log.addInfo("FileSM:handleAgentResponset: callNext ??" + sm_state + " ERROR='" + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) + "'");
				}
			} finally {
				FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
			}
		}
		}
		  }finally{
			  if (callNext) {
					FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, profileFactory, null);
				}
		  }
		
	}
private Element createMediaDownloadElement() {
		Element downloadEl = DocumentHelper.createElement("Download");
		Element mediaEl = DocumentHelper.createElement("Media");
		mediaEl.addAttribute("format", "");
		mediaEl.addAttribute("state", "NotReady");
		mediaEl.addAttribute("stale", "");
		downloadEl.add((Element)mediaEl.detach());
		return downloadEl;
	}

private void clearGroupFileCache(MRRequest request, XMPPI xmpp, MRDB db, String fileNID, String requestName, MRLog log,MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
 	
	Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
	Document requestDoc = (Document) requestDocTemplate.clone();
	MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
	MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
	Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
	Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
	Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
	MRXMLUtils.addAttribute(requestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, fileNID);
	MRXMLUtils.addElement(requestDataEl, requestEl);
	ClearDownloadCacheRequestHandler cacheHandler = new ClearDownloadCacheRequestHandler();
	MRRequest cacheRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());
	cacheRequest.setSuperUser(true); //to give permission to internal calls where userjid is manamentserver.localhost
	cacheHandler.getResponse(cacheRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
}

}
