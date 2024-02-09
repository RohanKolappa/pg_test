package com.ipvs.sessionservice.resource.handler;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.utils.Utils;

public class PublishFileDownloadStateRequestHandler extends FileBaseRequestHandler {
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
			throws Exception {
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
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	
		log.addInfo("PublishFileDownloadStateRequestHandler:: getResponse");
		Document requestDocument = request.getMessage().getDocument();
		Element publishFileElement = MRXMLUtils.getElementXpath(requestDocument, "//PublishFileDownloadState");
		String fileNID = MRXMLUtils.getAttributeValue(publishFileElement, "byNID");
		String mediaFormat = MRXMLUtils.getAttributeValue(publishFileElement, "format");
		String streamType = MRXMLUtils.getAttributeValue(publishFileElement, "streamType");
		
		String[] arrWhereXpath = { "//Internal/@fileNID='" + fileNID + "' and @queue_state > -1 and @state != 'completed' " };
		// do db read
		Element queueEL = FileSMUtils.readQueueElement_db_whereXpath(request, db, log, arrWhereXpath);
		String prepareFileName = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@fileName", "").trim();
		
		log.addInfo("PublishFileDownloadStateRequestHandler NID::" + fileNID + " format::" + mediaFormat);

		AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
		Element ApiKeyTableEl = appServerAgent.getAPIKey();
		String timestamp = HttpAuthentication.INFINITE_EXPIRY;
		String signature = HttpAuthentication.generateExpiryBasedSignature(ApiKeyTableEl, timestamp);
		
		String operatorJID = MRXMLUtils.getAttributeValue(queueEL, "ownerJID");
		String folderPath = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@folderPath", "").trim();
		
		if ((operatorJID == null) || (operatorJID.trim().length() == 0)) {
			throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "operator JID Not Found");
		}
		
		String startOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@startOffset", "").trim();
		String endOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@endOffset", "").trim();
		String title = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@title", "").trim();
		String new_UID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "").trim();
		
		String persistence = SessionServiceResourceConst.PERSISTENCE_PERMANENT;
		if(!startOffset.isEmpty() && !endOffset.isEmpty())
			persistence = SessionServiceResourceConst.PERSISTENCE_TEMPORARY;
		
		String apiKey = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");
		String dstTitle = ServiceAgentUtils.getAgentName(operatorJID);	
		Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log, xmpp, db, serviceAgentFactory, request, ApiKeyTableEl, dstTitle, request.getUserJID());
		String msIPAddress =  MRXMLUtils.getAttributeValue(appResourceInfo, "ipAddress","");
		String dstNID = MRXMLUtils.getAttributeValue(appResourceInfo, "dstNID","");		
		
		String downloadURL = "https://" + msIPAddress + "/controller/File/Download?mediaID=" + fileNID + "&timestamp=" + timestamp + "&apiKey=" + apiKey + "&signature=" + signature + "&format=" + mediaFormat.toLowerCase();
		downloadURL = downloadURL + "&persistence=" + persistence;
		downloadURL = downloadURL + "&type=" + SessionServiceResourceConst.EXPORT_TYPE;
		downloadURL = downloadURL + "&DstPort.portNID=" + dstNID;
		
		if(!new_UID.isEmpty())
			downloadURL = downloadURL + "&uuid=" + new_UID;
		
		log.addInfo("PublishFileDownloadStateRequestHandler:: Download Url::" + downloadURL);
		
		String requestName = "StartDownloadingRequest";
		Document downloadDocument = getRequestDocument(fileNID, mediaFormat, requestName, log, folderPath, streamType,downloadURL, title, new_UID, "export");
		sendRequestToAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, downloadDocument, MRConst.FTP_SERVICE, operatorJID, MRConst.FTP_SERVICE, this
				.getClass().getName());		
		return null;
	}

	private Document getRequestDocument(String fileNID, String mediaFormat, String requestName, MRLog log, String folderPath, String streamType, String downloadURL, String title, String new_UID, String type) throws Exception {
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
		MRXMLUtils.addAttribute(requestEl, "byNID", fileNID);
		MRXMLUtils.addAttribute(requestEl, "format", mediaFormat);
		MRXMLUtils.addAttribute(requestEl, "folderPath", folderPath);
		MRXMLUtils.addAttribute(requestEl, "streamType", streamType);
		MRXMLUtils.addAttribute(requestEl, "url", downloadURL);
		MRXMLUtils.addAttribute(requestEl, "title", title);
		MRXMLUtils.addAttribute(requestEl, "new_UID", new_UID);
		MRXMLUtils.addElement(requestEl, "type", type);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		return requestDoc;
	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document agentResponseDoc = request.getResult();
		String clientData = MRXMLUtils.getValueXpath(agentResponseDoc, "//" + MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
		
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);
		Document userResponseDoc = userRequest.createResponseData(); 
		log.addInfo("PublishFileDownloadStateRequestHandler handleAgentResponse");
		String fileNID = MRXMLUtils.getValueXpath(agentResponseDoc, "//FileNID", "");
		String requestName = MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		String transition = null;
		Element queueEL = null;
		boolean callNext = false;
		String error = null;
		int errorCode = -1;
		// create user request	
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);

		try {

			String[] arrWhereXpath = { "//Internal/@fileNID='" + fileNID + "' and @queue_state > -1 and @state != 'completed' " };
			// do db read
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
				userResponseDoc = userRequest.createResponseData();
			}

			int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""), -1);
			if (queue_state != FileSMQueue.QUEUE_STATUS_RUNNING) {
				log.addInfo("FileSM:handleAgentResponse ABORT:: already done or timed out ?? queue_state " + queue_state + " " + queueEL.asXML(), MRLog.OK, MRLog.NOTIFY);
				callNext = true;
				return;
			}

			String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim();
			transition = FileSMUtils.getTransitionFromEvent(request, xmpp, log, db, FileSM.TYPE_EXPORT, queueEL);
			if (transition != null) {

				FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, queueState,
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
						log.addInfo("FileSM:handleAgentResponset: callNext ??" + sm_state + " ERROR='" + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError())
								+ "'");
					}
				} finally {
					FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
				}
			}

		} finally {
			if (callNext) {
				FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler);
			}
		}
		MRException exception = null;
		String logData = "";
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			int exceptionCode = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
			String description = MRXMLUtils.getValueXpath(agentResponseDoc, "//Description", "AgentResponseFailed");
			exception = new MRException(exceptionCode, description);
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exceptionCode), "Description", description);
		}
		userRequest.setResult(userResponseDoc);
		userRequest.setState(state);
		userRequest.setException(exception);
		userRequest.addAuditEvent(this.getAuditEvent(userRequest, xmpp, log, db, AuditEventUtils.getAuditEventStatus(userRequest), AuditEventUtils.getAuditEventLevel(userRequest),
				AuditEventUtils.getAuditEventErrorDescription(userRequest)));		
		responseHandler.handleResponse(userRequest, logData);
	}

}
