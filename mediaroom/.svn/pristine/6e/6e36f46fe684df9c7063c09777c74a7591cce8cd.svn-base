package com.ipvs.proxy.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.proxy.app.ProxyClient;
import com.ipvs.proxy.impl.ProxyClientAppUtils.ActionEventList;
import com.ipvs.proxy.impl.ProxyClientAppUtils.RequestNameList;
import com.ipvs.utils.Utils;

public class ProxySession {

	
	private String roomNID;
	private String streamNID;
	private String recordID;
	private int streamState;
	private String fileState;
	private String sourceNID;
	private String fileNID;
	private Element profileXMLEL;
	private String proxyRequestInProgress;
	private int sessionState;
	private Document requestDoc;
	private ProxyClient proxyClient = null;
	private String dirDeviceElementName = null;
	private String storeOwnerJID = null;
    private String mediaStorePortResourceNID = null;
    private String parentMediaDirNID = null;
	
	public static final int WAIT_FOR_CREATE_RECORDING_RESPONSE = 1;
	public static final int WAIT_FOR_ADD_DEST_FILE_RESPONSE = 2;
	public static final int WAIT_FOR_CREATE_CONNECTION_RESPONSE = 3;
	public static final int WAIT_FOR_ADD_STREAM_ACTION_RESPONSE = 5;
	public static final int WAIT_FOR_STREAM_STATUS_EVENT = 4;
	public static final int WAIT_FOR_DELETE_MEDIA_ROOM_RESPONSE = 5;
	public static final int STATE_RECORDING = 6;
	public static final int STATE_DONE = 7;
	public static final int WAIT_FOR_ADD_TRACK_ACTION_RESPONSE = 8;
	public static final int WAIT_FOR_STREAM_ACTION_RESPONSE = 9;
	public static final int WAIT_FOR_PUB_SUB_EVENT = 10;
	public static final int WAIT_FOR_GET_DISK_SPACE_RESPONSE = 11;
	public static final int WAIT_FOR_SERVER_TIME_RESPONSE = 12;
	public static final int STATE_NOT_RECORDING = 0;
	
 
	public ProxySession(ProxyClient proxyClient) {
		this.proxyClient = proxyClient;
	}
	
	public synchronized String getRoomNID() {
		return roomNID;
	}
	public synchronized void setRoomNID(String roomNID) {
		this.roomNID = roomNID;
	}
	public synchronized String getStreamNID() {
		return streamNID;
	}
	public synchronized void setStreamNID(String streamNID) {
		this.streamNID = streamNID;
	}
	public synchronized String getRecordID() {
		return recordID;
	}
	public synchronized void setRecordID(String recordID) {
		this.recordID = recordID;
	}
	public synchronized int getStreamState() {
		return streamState;
	}
	public synchronized void setStreamState(int streamState) {
		this.streamState = streamState;
	}
	public synchronized String getFileState() {
		return fileState;
	}
	public synchronized void setFileState(String fileState) {
		this.fileState = fileState;
	}
	public synchronized String getSourceNID() {
		return sourceNID;
	}
	public synchronized void setSourceNID(String sourceNID) {
		this.sourceNID = sourceNID;
	}
	public synchronized String getFileNID() {
		return fileNID;
	}
	public synchronized void setFileNID(String fileNID) {
		this.fileNID = fileNID;
	}
	public synchronized Element getProfileXMLEL() {
		return profileXMLEL;
	}
	public synchronized void setProfileXMLEL(Element profileXMLEL) {
		this.profileXMLEL = profileXMLEL;
	}
	public synchronized String getProxyRequestInProgress() {
		return proxyRequestInProgress;
	}
	public synchronized void setProxyRequestInProgress(String proxyRequestInProgress) {
		//System.out.println("In set proxy Request in progress");
		this.proxyRequestInProgress = proxyRequestInProgress;
	}
	public synchronized int getSessionState() {
		return sessionState;
	}
	public synchronized void setSessionState(int sessionState) {
		this.sessionState = sessionState;
	}
	public synchronized Document getRequestDoc() {
		return requestDoc;
	}
	public synchronized void setRequestDoc(Document requestDoc) {
		this.requestDoc = requestDoc;
	}
	
	
	
	
	/************End Getters/Setters*********************/
	
	
	
	public void handleTransition(Document responseDoc,
			MRDB db, MRLog log, String userJID,
			XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String requestName = MRXMLUtils.getValueXpath(responseDoc,
				"//requestname", "");
		
		switch (RequestNameList.valueOf(requestName)) {
		case AddTrackActionRequest:
			log.addInfo(":Received AddTrackActionResponse", MRLog.INFO,MRLog.NOTIFY);
			if(this.getSessionState() != ProxySession.WAIT_FOR_ADD_TRACK_ACTION_RESPONSE) {
				return;
			}
			ProxyClientAppUtils.handleMediaRoomResponseMessages(responseDoc, 
					ProxyConstants.ADD_TRACK_ACTION_REQUEST,  proxyClient, log, db,
					userJID, xmpp, resourceFactory, serviceAgentFactory);
			log.addInfo(":Waiting for Stream Action Response", MRLog.INFO,MRLog.NOTIFY);
			this.setSessionState(ProxySession.WAIT_FOR_STREAM_ACTION_RESPONSE);
			break;

		case AddStreamActionRequest:
			log.addInfo(":Received AddStreamActionResponse", MRLog.INFO,MRLog.NOTIFY);
			if(this.getSessionState() != ProxySession.WAIT_FOR_ADD_STREAM_ACTION_RESPONSE) {
				return;
			}
			this.setSessionState(ProxySession.WAIT_FOR_STREAM_STATUS_EVENT);
			log.addInfo(":Waiting for Stream Status Event",MRLog.INFO,MRLog.NOTIFY);
			ProxyClientAppUtils.handleMediaRoomResponseMessages(responseDoc, 
					ProxyConstants.ADD_STREAM_ACTION_REQUEST,
					proxyClient, log, db, userJID,
					xmpp, resourceFactory, serviceAgentFactory);

			
			break;

		case DeleteMediaRoomRequest:
			log.addInfo(":Received DeleteMediaRoomResponse ", MRLog.INFO,MRLog.NOTIFY);
			ProxyClientAppUtils.recordingSession.setStreamNID(null);
			if(this.getSessionState()!= ProxySession.STATE_DONE) {
				this.setSessionState(ProxySession.WAIT_FOR_PUB_SUB_EVENT);
				log.addInfo(":Waiting for pubsub event", MRLog.INFO,MRLog.NOTIFY);
			}	
			ProxyClientAppUtils.handleMediaRoomResponseMessages(responseDoc, 
					ProxyConstants.DELETE_MEDIAROOM_REQUEST,
					proxyClient, log, db, userJID,
					xmpp, resourceFactory, serviceAgentFactory);

			break;

		case CreateRecordingRequest:
			
			log.addInfo(":Receiving CreateRecordingResponse", MRLog.INFO,MRLog.NOTIFY);
			if(this.getSessionState() != WAIT_FOR_CREATE_RECORDING_RESPONSE) {
				return;
			}
			//System.out.println(MRXMLUtils.documentToString(responseDoc));
			//recordID = MRXMLUtils.getValueXpath(responseDoc, "//recordID", "");
			String destNID = MRXMLUtils.getValueXpath(responseDoc, "//destNID", "");
			Element destDirEL = MRXMLUtils.getElementXpath(responseDoc, "//MediaStreamDirResource");
			Document requestDoc = null;
			requestDoc = this.getRequestDoc();
			if(this.getProxyRequestInProgress() == null || !this.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)) {
				return;
			}
			
			else {
				String strCode = MRXMLUtils
			.getValueXpath(responseDoc, "//Code", "");
			int code = Utils.getIntValue(strCode, Integer.valueOf(0));

			if (code != MRLog.OK) {
				String description = MRXMLUtils.getValueXpath(responseDoc,
						"//Description", "");
				ProxyClientAppUtils.generateProxyFailureMessage(code, description,
						ProxyConstants.STARTRECORDINGRESPONSE, userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);

				break;
			}
				String sourceNID = MRXMLUtils.getValueXpath(responseDoc, "//sourceNID", "");
				Element profileXML = MRXMLUtils.getElementXpath(responseDoc, "//V2DMediaStreamProfile");
				this.setSourceNID(sourceNID);
				this.setProfileXMLEL(profileXML);
			}
			Element mediaStorePortResourceNIDElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(MRXMLUtils.elementToDocument(destDirEL), true,db, log);			
			String  storeOwnerJID = MRXMLUtils.getValueXpath(responseDoc, "//storeOwnerJID", "");
			this.setStoreOwnerJID(storeOwnerJID);			
			setDirDeviceElementName(MRXMLUtils.getAttributeValue(mediaStorePortResourceNIDElement,"deviceElementId"));

			if(destNID!= null && (destNID.trim().equals("")|| destNID.trim().equals("null"))) {
				mediaStorePortResourceNID = MRXMLUtils.getValueXpath(destDirEL,"MediaStorePortResourceNID", "");
			    parentMediaDirNID = MRXMLUtils.getAttributeValue(destDirEL, "NID"); 
				MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, "1234", "serviceName", requestName, new MRMessage(requestDoc), "");
				ProxyClientAppUtils.sendGetServerTimeRequest(request, xmpp, log, serviceAgentFactory);
				this.setSessionState(ProxySession.WAIT_FOR_SERVER_TIME_RESPONSE);
				log.addInfo(":Waiting for server time response", MRLog.INFO,MRLog.NOTIFY);
			}
			else {
				this.setFileNID(destNID);
				String sourceNID = this.getSourceNID();
				Element profileXMLEL = this.getProfileXMLEL();
				MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, "1234", "serviceName", requestName, new MRMessage(requestDoc), "");
				ProxyClientAppUtils.sendCreateConnectionRequest(request, sourceNID, destNID, profileXMLEL,userJID, xmpp, log, db, resourceFactory,serviceAgentFactory);
				this.setSessionState(ProxySession.WAIT_FOR_CREATE_CONNECTION_RESPONSE);
				log.addInfo(":Waiting for add create connection response", MRLog.INFO,MRLog.NOTIFY);
			}
			 break;
		case GetServerTimeRequest:
			long serverTime = 0;
			try {
				serverTime = Long.valueOf(MRXMLUtils.getValueXpath(responseDoc, "//serverTime", ""));
			}catch(Exception e) {
				e.printStackTrace();
				log.addInfo(": Received Invalid Server Time ", MRLog.INFO,MRLog.NOTIFY);
				serverTime = System.currentTimeMillis();
			}
			Element metaDataEL = MRXMLUtils.newElement("MetaData");
			requestDoc = this.getRequestDoc();
			MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, "1234", "serviceName", requestName, new MRMessage(requestDoc), "");
			metaDataEL = ProxyClientAppUtils.generateMetaData(requestDoc.getRootElement(), metaDataEL, serverTime, db, log, request);
			ProxyClientAppUtils.sendAddDestFileRequest(request, mediaStorePortResourceNID, parentMediaDirNID,  metaDataEL, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);
			this.setSessionState(ProxySession.WAIT_FOR_ADD_DEST_FILE_RESPONSE);
			log.addInfo(":Waiting for add destination file response", MRLog.INFO,MRLog.NOTIFY);
			break;
			
		case AddDestFileRequest:
			log.addInfo(":Received Add Destination File Response", MRLog.INFO,MRLog.NOTIFY);
			if(this.getSessionState() != ProxySession.WAIT_FOR_ADD_DEST_FILE_RESPONSE) {
				return;
			}

			String strCode = MRXMLUtils.getValueXpath(responseDoc, "//Code", "");
			int code = Utils.getIntValue(strCode, Integer.valueOf(0));

			if (code != 0) {
				String description = MRXMLUtils.getValueXpath(responseDoc,
						"//Description", "");
				ProxyClientAppUtils.generateProxyFailureMessage(code, description,
						ProxyConstants.STARTRECORDINGRESPONSE, userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);

				break;
			}

			Element recordIDEL = MRXMLUtils.getElementXpath(responseDoc, "//Tag[@Name='ID']");
			if(recordIDEL!=null) {
				recordID = MRXMLUtils.getValue(recordIDEL);
			}	
			if(this.getProxyRequestInProgress() == null || !this.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)) {
				return;
			}
			

			Element mediaStreamFileResourceDoc = MRXMLUtils.getElementXpath(responseDoc, "//MediaStreamFileResource");
			destNID = MRXMLUtils.getAttributeValue(mediaStreamFileResourceDoc,"NID");
			this.setFileNID(destNID);
			String sourceNID = this.getSourceNID();
			Element profileXMLEL = this.getProfileXMLEL();
			request = MRRequest.createRequest(userJID, MRLog.INFO, "1234", "serviceName", requestName, new MRMessage(null), "");
			ProxyClientAppUtils.sendCreateConnectionRequest(request, sourceNID, destNID, profileXMLEL,userJID, xmpp, log, db, resourceFactory,serviceAgentFactory);
			this.setSessionState(ProxySession.WAIT_FOR_CREATE_CONNECTION_RESPONSE);
			log.addInfo(":Waiting for add create connection response", MRLog.INFO,MRLog.NOTIFY);
			break;
			
		case CreateConnectionRequest:
			log.addInfo(":Received create connection response", MRLog.INFO,MRLog.NOTIFY);
			if(this.getSessionState() != ProxySession.WAIT_FOR_CREATE_CONNECTION_RESPONSE) {
				return;
			}
			//System.out.println(MRXMLUtils.documentToString(responseDoc));
			//fileNID = this.getFileNID();
			strCode = MRXMLUtils.getValueXpath(responseDoc, "//Code", "");
			code = Utils.getIntValue(strCode, Integer.valueOf(0));
			if (code != 0) {
				String description = MRXMLUtils.getValueXpath(responseDoc,
						"//Description", "");
				ProxyClientAppUtils.generateProxyFailureMessage(code, description,
						ProxyConstants.STARTRECORDINGRESPONSE, userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);

				break;
			}
			String streamNID = MRXMLUtils.getValueXpath(responseDoc,
					"//streamNID", "");
			this.setStreamNID(streamNID);
			String roomNID = MRXMLUtils.getValueXpath(responseDoc, "//roomNID",
			"");
			this.setRoomNID(roomNID);
			ProxyClientAppUtils.sendStartRecordStream(streamNID, null, MRRequest
				.createRequest(userJID), userJID, xmpp, log, db,
				resourceFactory, serviceAgentFactory);
				this.setStreamState(MediaRoomServiceConst.STREAM_STARTED);
				this.setSessionState(ProxySession.WAIT_FOR_ADD_STREAM_ACTION_RESPONSE);
				log.addInfo(":Waiting for AddSreamActionResponse", MRLog.INFO,MRLog.NOTIFY);
				break;
		default:
			return;	

		}

	}
	
	public void handleEvent(Document notifyMessageDoc, String action,
			String nodeID, MRDB schema, MRLog log, String userJID, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String fromJID = null;
		//int state = 0;

		//boolean sendProxyAuthNotifyMessage = false;
		boolean sendProxyRecordResponseMessage = false;
		String streamNID = null;
		String fileNID = null;
		String fileStatus = null;
		//String roomUser = null;
		String proxyResponseType = null;
		String responseStatus = null;
		streamNID = MRXMLUtils.getValueXpath(notifyMessageDoc,
				"//streamNID", "");
		String recordingSessionStreamNID = getStreamNID();

		//fix to don't handle the stream events which are not belonging to current recording session
		if (recordingSessionStreamNID != null && !streamNID.equals(recordingSessionStreamNID)) {
			log.addInfo("Ignore Stream Event which are not part of recording session. StreamNID :="+streamNID +" RecordingStreamNID :="+getStreamNID(), MRLog.INFO,MRLog.NOTIFY);
			return;
			
		}
		
		switch (ActionEventList.valueOf(action)) {
		
		case StopOutput:
		case StartOutput:
		case NotifyResponse:
			break;

		case UserPresence:
			
			if(this.proxyRequestInProgress!=null){
				return;
			}
			fromJID = MRXMLUtils.getAttributeValueXpath(notifyMessageDoc,
					"//presence", "from", "");

			if ((fromJID == null) || (fromJID.length() == 0)) {
				return;
			}

			String fromRoomNID = ServiceAgentUtils.getRoomNID(fromJID);

			if ((fromRoomNID == null) || (fromRoomNID.length() == 0)) {
				return;
			}
			if(fromRoomNID.equalsIgnoreCase(roomNID)) {
				this.proxyRequestInProgress = ProxyConstants.RECORDINGSTATUSREQUEST;
				MRRequest request = MRRequest.createRequest(userJID);
				ProxyClientAppUtils.deleteMediaRoom(roomNID, request, log, schema, xmpp, resourceFactory, serviceAgentFactory);
				setFileState(MediaStoreServiceConst.RECORDING_STOPPED);
				proxyResponseType = ProxyConstants.RECORDINGSTATUSRESPONSE;
				String message = proxyResponseType + ProxyConstants.SEPERATOR + this.fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + ProxyExceptionCodes.USER_LOGGED_OFF  + ProxyConstants.SEPERATOR + ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.USER_LOGGED_OFF) + ProxyConstants.SEPERATOR + "<RequestDoc />"+ ProxyConstants.SEPERATOR;
				ProxyClientAppUtils.sendGetDestDirListRequest(MRRequest.createRequest(userJID), message, userJID, xmpp, log, schema, resourceFactory, serviceAgentFactory);
			}	
			 
			break;

		case HandleXMPPEvent:

			log.addInfo(":Received pubsub event", MRLog.INFO,MRLog.NOTIFY);
			if(nodeID!= null && !nodeID.equalsIgnoreCase(this.getFileNID())) {
				return;
			}

	
				fileNID = nodeID;

				fileStatus = ProxyClientAppUtils.getProxyFileStatus(fileNID, schema, log, userJID);
				this.setStreamState(MediaRoomServiceConst.STREAM_STOPPED);
				log.addInfo("FileAction=" + fileStatus, MRLog.DEBUG,
						MRLog.NOTIFY);

				if (fileStatus == null) {
					return;
				}

				action = fileStatus;
	

			break;

		case StreamStatusEvent:
			Element dataEL = MRXMLUtils.getElementXpath(notifyMessageDoc, "//Data");
			String eventCode = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(dataEL), "//eventCode", "");
			if(Integer.valueOf(eventCode).intValue() == MediaRoomServiceConst.RECORDING_ERROR) {
				String message = this.fileNID + ProxyConstants.SEPERATOR + this.recordID + ProxyConstants.SEPERATOR +  ProxyExceptionCodes.RECORDING_ERROR + ProxyConstants.SEPERATOR + ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.RECORDING_ERROR);
				proxyResponseType = ProxyConstants.RECORDINGSTATUSRESPONSE;
				this.proxyRequestInProgress = ProxyConstants.RECORDINGSTATUSREQUEST;
				ProxyClientAppUtils.deleteMediaRoom(roomNID, MRRequest.createRequest(userJID), log, schema, xmpp, resourceFactory, serviceAgentFactory);
				ProxyClientAppUtils.sendProxyResponseMessage(proxyResponseType, message, userJID, proxyClient,  log,  schema,  xmpp,  resourceFactory,  serviceAgentFactory);
				return;
			}
			log.addInfo(":Received StreamStatusEvent", MRLog.INFO,MRLog.NOTIFY);
			fileNID = this.getFileNID();
			if(this.getSessionState() != ProxySession.WAIT_FOR_STREAM_STATUS_EVENT) {
				return;
			}
			if(Integer.valueOf(eventCode).intValue() == MediaRoomServiceConst.RECORDING_STARTED) {				
				fileState = MediaStoreServiceConst.RECORDING_STARTED;
				if(this.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)) {
					action = ProxyConstants.RECORD_STARTED;
				}
				else if (this.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.RESUMERECORDINGREQUEST)) {
					action = ProxyConstants.RECORD_RESUMED;
				}
					
			} else if(Integer.valueOf(eventCode).intValue() == MediaRoomServiceConst.RECORDING_STOPPED) {
				if(this.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.PAUSERECORDINGREQUEST)) {
					action = ProxyConstants.RECORD_PAUSED;
					fileState = MediaStoreServiceConst.RECORDING_PAUSED;
				}else {
					fileState = MediaStoreServiceConst.RECORDING_STOPPED;
				}
			}
			else {
				String destinationEventActionData = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(dataEL), "//destinationEventActionData", "");
				ProxyException pe = ProxyClientAppUtils.generateProxyException(Integer.valueOf(eventCode), destinationEventActionData);
				proxyResponseType = MRRequest.getResponseName(this.proxyRequestInProgress);
				ProxyClientAppUtils.generateProxyFailureMessage(pe.getCode(), pe.getData(), proxyResponseType, userJID, proxyClient, log, schema, xmpp, resourceFactory, serviceAgentFactory);
			}
			
			
			break;	
		default:
			return;
		}
		

		switch (ActionEventList.valueOf(action)) {
		
		case StopOutput:
			ProxyClientAppUtils.validatePauseResumeRecordingAgentResponseMessage(proxyClient, log, schema,
					userJID, xmpp, resourceFactory, serviceAgentFactory,
					notifyMessageDoc,
					ProxyConstants.PAUSERECORDINGRESPONSE,
					ProxyConstants.PAUSERECORDINGREQUEST);
			this.setSessionState(WAIT_FOR_STREAM_STATUS_EVENT);
			log.addInfo(":Waiting for StreamStatusEvent", MRLog.INFO,MRLog.NOTIFY);
			break;

		case StartOutput:
			ProxyClientAppUtils.validatePauseResumeRecordingAgentResponseMessage(proxyClient, log, schema,
					userJID, xmpp, resourceFactory, serviceAgentFactory,
					notifyMessageDoc,
					ProxyConstants.RESUMERECORDINGRESPONSE,
					ProxyConstants.RESUMERECORDINGREQUEST);
			this.setSessionState(WAIT_FOR_STREAM_STATUS_EVENT);
			log.addInfo(":Waiting for StreamStatusEvent", MRLog.INFO,MRLog.NOTIFY);
			break;

		case NotifyResponse:
			log.addInfo(":NotifyResponse", MRLog.INFO,MRLog.NOTIFY);
			streamNID = MRXMLUtils.getValueXpath(notifyMessageDoc,
					"//streamNID", "");

			if (this.getStreamNID()!=null && getStreamNID().equalsIgnoreCase(streamNID)) {
				responseStatus = MRXMLUtils.getValueXpath(notifyMessageDoc,
						"//status", "");

				if (Utils.getIntValue(responseStatus, MRLog.OK) != MRLog.OK) {
					ProxyClientAppUtils.validateStartRecordingAgentResponseMessage(proxyClient, log, schema,
							userJID, xmpp, resourceFactory,
							serviceAgentFactory,  notifyMessageDoc,
							action, streamNID, responseStatus);
				}
			}

			break;

		case RecordStarted:
			proxyResponseType = ProxyConstants.STARTRECORDINGRESPONSE;
			sendProxyRecordResponseMessage = true;

			break;

		case RecordResumed:
			proxyResponseType = ProxyConstants.RESUMERECORDINGRESPONSE;
			sendProxyRecordResponseMessage = true;

			break;

		case RecordPaused:
			proxyResponseType = ProxyConstants.PAUSERECORDINGRESPONSE;
			sendProxyRecordResponseMessage = true;

			break;

		case RecordStopped:
			proxyResponseType = ProxyConstants.STOPRECORDINGRESPONSE;
			sendProxyRecordResponseMessage = true;
			break;
		case Error:
			proxyResponseType = MRRequest.getResponseName(this.proxyRequestInProgress);
			ProxyClientAppUtils.generateProxyFailureMessage(ProxyExceptionCodes.MEDIASTORE_ERROR, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.MEDIASTORE_ERROR), proxyResponseType, userJID, proxyClient, log, schema, xmpp, resourceFactory, serviceAgentFactory);
			return;
		default:
			return;
		}

		/*if (sendProxyAuthNotifyMessage) {
			ProxyClientAppUtils.sendProxyResponseMessage(ProxyConstants.AUTHNOTIFY, 
					roomUser + ProxyConstants.SEPERATOR + state, userJID, proxyClient, log, schema, xmpp, resourceFactory, serviceAgentFactory);

		}*/

		if (sendProxyRecordResponseMessage) {
			fileNID = null;
			fileNID = this.getFileNID();
			String clientData = dirDeviceElementName + ProxyConstants.SEPERATOR + proxyResponseType + ProxyConstants.SEPERATOR +  fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + ProxyExceptionCodes.NOERROR  + ProxyConstants.SEPERATOR + ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.NOERROR) + ProxyConstants.SEPERATOR;
			ProxyClientAppUtils.sendDeviceStatusRequest(storeOwnerJID, clientData, xmpp, log, schema, resourceFactory, serviceAgentFactory, userJID);
			this.setSessionState(ProxySession.WAIT_FOR_GET_DISK_SPACE_RESPONSE);
			log.addInfo(": Waiting for GetDiskSpaceResponse", MRLog.INFO,MRLog.NOTIFY);
			
			
		}
	}

	public String getDirDeviceElementName() {
		return dirDeviceElementName;
	}

	public void setDirDeviceElementName(String dirDeviceElementName) {
		this.dirDeviceElementName = dirDeviceElementName;
	}

	public String getStoreOwnerJID() {
		return storeOwnerJID;
	}

	public void setStoreOwnerJID(String storeOwnerJID) {
		this.storeOwnerJID = storeOwnerJID;
	}

	
	
}
