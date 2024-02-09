package com.ipvs.assetadminservice.file.handler;

import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.UpgradeFileRequestHandler;
import com.ipvs.utils.Utils;

public class PublishImportFileRequestHandler extends BasePublishFileRequestHandler implements MRRequestHandlerI {

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
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

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		Document requestDocument = request.getMessage().getDocument();
		List<?> listOfFiles  =  MRXMLUtils.getListXpath(requestDocument, "//File");

		//read all the list from database
		Document resourceDoc = getData(request, xmpp, log, db, resourceFactory, profileFactory, requestDocument);
		Document responseDoc = request.createResponseData();
		//List<Element> responseFileList = new ArrayList<Element> ();
		Hashtable<String,Document> groupsFileCache = new Hashtable<String, Document>();
		for(Object object:listOfFiles) {
			Element fileEl = (Element) object;		
			String queueID = addOrUpdateFile(request, xmpp, log, db, resourceFactory, profileFactory, responseDoc, resourceDoc, fileEl, groupsFileCache);
			String mediaType = MRXMLUtils.getValueXpath(fileEl, ".//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
			Element mediaStreamFileEl = getMediaStreamFileFromFileElement(fileEl, resourceDoc, groupsFileCache,mediaType);
			Element mediaStoreFileEl = MRXMLUtils.getElement(fileEl, AMConst.MEDIA_STORE_FILE);
			Element storeFileMediaGroupEL = MRXMLUtils.getElementXpath(mediaStoreFileEl, ".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);
			String mediaGroupID = MRXMLUtils.getValue(storeFileMediaGroupEL);
			String fileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID,"");  
			String fileState = MRXMLUtils.getValueXpath(fileEl,".//"+MediaStoreServiceConst.STATE_EL_NAME,"");
			int state = (fileState.equals(SessionServiceConst.READY) ? MediaStoreServiceConst.SUCCESS_STATE : MediaStoreServiceConst.FAILURE_STATE);
			updateQueueStatus(queueID, state, fileNID,mediaGroupID, mediaType, request, xmpp, log, db, resourceFactory, profileFactory);
		}
		return responseDoc;
	}

	private void updateQueueStatus(String queueID, int state, String fileNID, String mediaGroupID, String mediaType, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception{
		String transition = null; 
		Element queueEL = null; 
		boolean callNext = false; 
		String error = null;
		int errorCode = -1;
		String requestName = request.getRequestName();
		log.addInfo("PublishImportFileRequestHandler.updateQueueStatus requestName :='" + requestName, MRLog.INFO, MRLog.NOTIFY);
		Semaphore lock = null;
		try{
			String[] arrWhereXpath = { "//Internal/@queueID='" + queueID + "'" };
			lock = IPVSMonitor.getMonitor().getLock(queueID, request.getNID());
			log.addInfo("PublishImportFileRequestHandler.updateQueueStatus acquire lock on queueID:"+queueID );
			queueEL = FileSMUtils.readQueueElement_db_whereXpath(request, db, log, arrWhereXpath);
			if (queueEL == null) {
				log.addInfo("FileSM:handleAgentResponse ABORT:requestName :='" + requestName + "' " + " ABORTING queueEL is NULL ", MRLog.INFO, MRLog.NOTIFY); 
				callNext = true; 
				return; 
			}
			if (state == MediaStoreServiceConst.FAILURE_STATE) { 
				error = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Description", ""); 
				if (error.equals("")) { 
					String action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim(); 
					error = "Error during " + requestName + " '" + FileSMUtils.getActionDescription(action, log) + "'"; 
				} 
				errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(queueEL), "//Exception/Code", ""), 
						AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION); 
				//userResponseDoc = userRequest.createResponseData(); 
			} 

			int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""), -1); 
			if (queue_state != FileSMQueue.QUEUE_STATUS_RUNNING) { 
				log.addInfo("FileSM:handleAgentResponse ABORT:: already done or timed out ?? queue_state " + queue_state + " " + queueEL.asXML(), MRLog.OK, MRLog.NOTIFY); 
				callNext = true; 
				return; 
			} 

			String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim(); 
			transition = FileSMUtils.getTransitionFromEvent(request, xmpp, log, db, FileSM.IMPORT_FILE_UNPACK_REQUEST, queueEL); 
			if (transition != null) { 

				FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, queueState, 
						this.getClass().getName()); 
				String sm_state="";
				try { 
					if (error != null) { 
						stateMachine.setErrorCode(errorCode); 
						stateMachine.setError(error); 
					} 
					// call handle transition 
					stateMachine.handleTransition(transition); 
					sm_state = stateMachine.getState(); 
					callNext = FileSM.STATE_COMPLETED.equals(sm_state) || stateMachine.getError() != null; 
					if (callNext) { 
						log.addInfo("FileSM:handleAgentResponset: callNext ??" + sm_state + " ERROR='" + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) 
								+ "'"); 
					} 
				} finally { 
					FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
					if(FileSM.STATE_COMPLETED.equals(sm_state)){
						upgradeImportedMedia(mediaGroupID, fileNID, mediaType, xmpp, request, log, db, resourceFactory, serviceAgentFactory);

					}
				} 
			} 


		}finally { 
			if (callNext) { 
				FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null); 
			}
			if(lock != null){
				IPVSMonitor.getMonitor().releaseLock(queueID, lock);
				log.addInfo("PublishImportFileRequestHandler.updateQueueStatus Release lock on queueID:"+queueID );
			}

		} 

	}

	public void upgradeImportedMedia(String mediaGroupID, String fileNID, String mediaType, XMPPI xmpp, MRRequest request, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory ) throws Exception{
		String upgradeFileRquestName = MRRequest.getRequestName(UpgradeFileRequestHandler.class.getName());
		Document upgradeFileSampleRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/"+ upgradeFileRquestName + ".xml", log);
		Element upgradeFileEL = MRXMLUtils.getElementXpath(upgradeFileSampleRequestDoc,"//UpgradeFile");
		if(mediaType!= null){
			Document mediaGroupFileDoc= MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
			if(mediaGroupFileDoc != null){
				fileNID = MRXMLUtils.getAttributeValue(mediaGroupFileDoc.getRootElement(), "NID");				
			}
		}
		log.addInfo("PublishImportFileRequestHandler.UpgradeFile : fileNID: "+fileNID);
		MRXMLUtils.setAttributeValue(upgradeFileEL, "byNID", fileNID);
		Document upgradeFileRequestDoc = SessionServiceUtils.getRequestXML(upgradeFileRquestName, log, upgradeFileEL);
		MRRequest upgradeFileRequest = MRRequest.createRequest(request, upgradeFileRequestDoc, upgradeFileRquestName,
				ServiceClient.SESSIONSERVICE_RESOURCE);
		UpgradeFileRequestHandler upgradeFileRequestHandler= new UpgradeFileRequestHandler();
		upgradeFileRequestHandler.getResponse(upgradeFileRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}

}
