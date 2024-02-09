package com.ipvs.proxy.impl;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.Date;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.DeviceAdminClientAgent;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.XAuthServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.proxy.app.ProxyClient;
import com.ipvs.proxy.handler.AuthorizationUpdateRequestHandler;
import com.ipvs.proxy.handler.CreateBookmarkRequestHandler;
import com.ipvs.proxy.handler.GetStreamListRequestHandler;
import com.ipvs.utils.Utils;

public class ProxyClientAppUtils {

	private static String sourceTitle = null;
	public  static ProxySession recordingSession = null;

	
	public static String getHandlerName(String requestName) {
		try { 
			ProxyRequestNameList.valueOf(requestName.toUpperCase());
			return "com.ipvs.proxy.handler." + requestName.trim() + "Handler";
		}catch(Exception e) {
			return "DefaultRequestHandler";
		}
		
	}

	public static boolean getRequestNameCase(String requestName) {
		
		try { 
			ProxyRequestNameList.valueOf(requestName.toUpperCase());
			return true;
		}catch(Exception e) {
			return false;
		}
		
		
		
	}

	public static void validateRequest(Document requestDoc, String requestName) throws Exception {

		Element el = requestDoc.getRootElement();
		String id = null;

		String streamNID = null;
		if (el != null) {
			id = MRXMLUtils.getValueXpath(el, "//ID", "");
			if(recordingSession!=null) {
				streamNID = recordingSession.getStreamNID();
			}	
		}
		boolean isValid = false;
		
		if(recordingSession == null) {
			if(requestName.equalsIgnoreCase(ProxyConstants.PAUSERECORDINGREQUEST) || requestName.equalsIgnoreCase(ProxyConstants.RESUMERECORDINGREQUEST) || requestName.equalsIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST) || requestName.equalsIgnoreCase(ProxyConstants.CREATEBOOKMARKREQUEST) || requestName.equalsIgnoreCase(ProxyConstants.MEDIATIMESTAMPREQUEST)) {
				throw new ProxyException(ProxyExceptionCodes.RECORDING_SESSION_NOT_FOUND, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.RECORDING_SESSION_NOT_FOUND));
			}
		}	
		if(recordingSession== null || recordingSession.getProxyRequestInProgress() == null) {
			isValid = true;
		}
		if(!isValid && !requestName.equalsIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST)  ) {
			throw new ProxyException(
					ProxyExceptionCodes.REQUEST_IN_PROCESS,
					ProxyExceptionCodes
					.getErrorDescription(ProxyExceptionCodes.REQUEST_IN_PROCESS));
		}
		

		// Checking the stream started or not for given record ID
		// Pause/Resume/CreateBookMark
		if (!requestName.equals(ProxyConstants.STOPRECORDINGREQUEST)
				&& isStreamStarted(id, requestName, streamNID)) {
			throw new ProxyException(
					ProxyExceptionCodes.STREAM_ID_NOT_FOUND,
					ProxyExceptionCodes
					.getErrorDescription(ProxyExceptionCodes.STREAM_ID_NOT_FOUND));
		}

	}

	public static ProxyException generateProxyException(int mediaroomErrorCode, String errorDescription) {
		String description = ProxyExceptionCodes.getErrorDescription(mediaroomErrorCode);
		if(description.equalsIgnoreCase(ProxyExceptionCodes.OTHER_DESCRIPTION)) {
			return new ProxyException(ProxyExceptionCodes.OTHER, mediaroomErrorCode + " : " + errorDescription);
		}	
		else {
			return new ProxyException (mediaroomErrorCode, errorDescription);
		}
	}
	
	public static StringTokenizer handleRequest(Document proxyRequestDoc, ProxyClient proxyClient,
			String userJID, XAuthServerAgent xAuthAgent, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Element el = MRXMLUtils.getRootElement(proxyRequestDoc);
		String id = MRXMLUtils.getValueXpath(el, "//ID", "");
		String requestID = el.element("header").elementText("requestID");
		String requestName = proxyRequestDoc.getRootElement().getName();		
		MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO,
				requestID, "serviceName", requestName, new MRMessage(proxyRequestDoc), "");
		StringTokenizer st = null;
		
		//move these validations and the session creation code to request handler.
		//APP Utils create the request object and hand over to handlers. Don't do any validation and session creations		
		validateRecordingSessionRequest(requestName,id);
		
		if(recordingSession == null && requestName.equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)) {
			recordingSession = new ProxySession(proxyClient);
			recordingSession.setRecordID(id);
		}	
		
		String fileNID = null;
		if(recordingSession!=null) {
			fileNID = recordingSession.getFileNID();
		}	
		Element mediaFileEl = null;
		if (fileNID != null) {
			
			if(ProxyClientAppUtils.getMediaStreamFileResource(fileNID, db, log, userJID)!=null) {
				mediaFileEl = ProxyClientAppUtils.getMediaStreamFileResource(fileNID, db, log,userJID).getRootElement();
			}	
		}
		try {
			MRRequestHandlerI handler = null;
			//int requestCase = getRequestNameCase(requestName);
			String handlerName = getHandlerName(requestName);
			if (handlerName.equalsIgnoreCase("DefaultRequestHandler")) {
				throw new ProxyException(
						ProxyExceptionCodes.UNSUPPORTED_REQUEST,
						ProxyExceptionCodes
						.getErrorDescription(ProxyExceptionCodes.UNSUPPORTED_REQUEST));
			}
			validateRequest(proxyRequestDoc, requestName);
			switch (ProxyRequestNameList.valueOf(requestName.toUpperCase())) {
			//case MEDIATIMESTAMPREQUEST:
				/*id = MRXMLUtils.getValueXpath(el, "//ID", "");
				st = generateSyncResponse(proxyRequestDoc, null,
						ProxyExceptionCodes.NOERROR, null, id, request,
						userJID, log, db, xmpp, resourceFactory,
						serviceAgentFactory);
				return st;*/
			case AUTHORIZATIONUPDATEREQUEST:
				handler = new AuthorizationUpdateRequestHandler(xAuthAgent);
				handler.getResponse(request, xmpp, log, db, resourceFactory,
						serviceAgentFactory);
				return generateAuthorizationUpdateResponse(proxyRequestDoc, null,
						ProxyExceptionCodes.NOERROR, "", null, request,
						userJID, log, db, xmpp, resourceFactory,
						serviceAgentFactory);
				
			case GETSTREAMLISTREQUEST:
				handler = new GetStreamListRequestHandler(xAuthAgent);
				Document responseDoc = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				String response = MRXMLUtils.getValueXpath(responseDoc, "//Response", "");
				response = URLDecoder.decode(response, "UTF-8");
				return new StringTokenizer(response,ProxyConstants.SEPERATOR);
			default:
				handler = (MRRequestHandlerI) Class.forName(handlerName)
				.newInstance();
			
			handler.getResponse(request, xmpp, log, db, resourceFactory,
					serviceAgentFactory);
			}

		} catch (ProxyException pe) {
			pe.printStackTrace();
			sendResponseMessage(proxyRequestDoc, mediaFileEl, pe.getCode(), pe
					.getData(), id, proxyClient, request, userJID, log, db, xmpp,
					resourceFactory, serviceAgentFactory);
		} catch (MRException me) {
			me.printStackTrace();
			ProxyException pe = ProxyClientAppUtils.generateProxyException(me.getCode(),me.getData());
			sendResponseMessage(proxyRequestDoc, mediaFileEl, pe.getCode(), pe.getData(), id, proxyClient, request, userJID, log, db, xmpp,
					resourceFactory, serviceAgentFactory);
		} catch (Throwable throwableObj) {
			throwableObj.printStackTrace();
			sendResponseMessage(proxyRequestDoc, mediaFileEl,
					ProxyExceptionCodes.OTHER, throwableObj.toString(), id,
					proxyClient, request, userJID, log, db, xmpp, resourceFactory,
					serviceAgentFactory);
		}
		return null;
	}

	@SuppressWarnings("fallthrough")
	private static void validateRecordingSessionRequest(String requestName,
			String id)  throws MRException,Exception {
		switch(ProxyRequestNameList.valueOf(requestName.toUpperCase())) {
		case STARTRECORDINGREQUEST:
		case STOPRECORDINGREQUEST:
		case RESUMERECORDINGREQUEST:
		case PAUSERECORDINGREQUEST:
		case RECORDINGSTATUSREQUEST:
			if(recordingSession != null) {
				if(recordingSession.getRecordID()!= null && !recordingSession.getRecordID().equalsIgnoreCase(id)) {
					//if(recordingSession.getSessionState() != ProxySession.STATE_DONE ) {
					throw new ProxyException(ProxyExceptionCodes.RECORDING_SESSION_IN_PROGRESS, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.RECORDING_SESSION_IN_PROGRESS));
					//}
					//else {
					//recordingSession = null;
					//}
				}

			}
		default :
			return;
		}

	}

	public static StringTokenizer handleRequest(Document proxyRequestDoc, ProxyClient proxyClient, String title , String userJID, XAuthServerAgent xAuthAgent , MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		Element el = null;
		String id = "unable to get id";
		MRRequest request = null;

		try {
			log.setLogLevel(MRLog.NOTIFY);

			
			
			String requestName = proxyRequestDoc.getRootElement().getName();
			boolean requestCase = getRequestNameCase(requestName);
			
			el = null;

			String requestID = null;

			if (requestCase) {
				
				//if (proxyRequestDoc != null) {
					el = proxyRequestDoc.getRootElement();
				//}
				id = MRXMLUtils.getValueXpath(el, "//ID", "");
				requestID = el.element("header").elementText("requestID");
				request = MRRequest.createRequest(userJID, MRLog.INFO,
						requestID, "serviceName", requestName, new MRMessage(
								proxyRequestDoc), "");
				return handleRequest(proxyRequestDoc, proxyClient, userJID, xAuthAgent , log, db, xmpp,
						resourceFactory, serviceAgentFactory);
			} else {
				
				//ProxyClientAppUtils.sendProxyResponseMessage(ProxyConstants.AUTHRESPONSE, MRXMLUtils.documentToString(proxyRequestDoc), userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);
				/*StringTokenizer response = handleResponseQueue(null, userJID, log, db, xmpp,
						resourceFactory, serviceAgentFactory);
				return response;*/
				return null;
				
			}
		}  catch (ProxyException pe) {
			pe.printStackTrace();
			sendResponseMessage(proxyRequestDoc, null, pe.getCode(), pe
					.getData(), id, proxyClient, request, userJID, log, db, xmpp,
					resourceFactory, serviceAgentFactory);
			return null;

		} catch (MRException me) {
			me.printStackTrace();
			ProxyException pe = ProxyClientAppUtils.generateProxyException(me.getCode(),me.getData());
			sendResponseMessage(proxyRequestDoc, null, pe.getCode(), pe
					.getData(), id, proxyClient, request, userJID, log, db, xmpp,
					resourceFactory, serviceAgentFactory);
			return null;
		} catch (Throwable throwableObj) {
			throwableObj.printStackTrace();
			sendResponseMessage(proxyRequestDoc, null,
					ProxyExceptionCodes.OTHER, throwableObj.toString(), id, proxyClient,
					request, userJID, log, db, xmpp, resourceFactory,
					serviceAgentFactory);
			return null;
		}
	}

	public synchronized static void deleteRecordingSession(String userJID, MRLog log, MRDB schema, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
		if(recordingSession == null) {
			return;
		}
		try {
			String roomNID = recordingSession.getRoomNID();
			if(roomNID!=null) {
				ProxyClientAppUtils.sendStopRecordingRequest(roomNID, MRRequest.createRequest(userJID),	log, schema, xmpp, resourceFactory, serviceAgentFactory);
			}
		}catch (Exception e) {
			e.printStackTrace();
		}
		recordingSession = null;
	}

	public synchronized static void addProxyRequestToRecordingSession(
			String proxyRequest, String fileNID, Document proxyRequestDoc) throws Exception{
		recordingSession.setRequestDoc(proxyRequestDoc);
		recordingSession.setProxyRequestInProgress(proxyRequest);
	}


	public synchronized static  void deleteProxyRequestMessageFromRecordingSession(
			String proxyRequest, String fileNID) {
		if(recordingSession!=null && proxyRequest.equalsIgnoreCase(recordingSession.getProxyRequestInProgress())) {
			recordingSession.setProxyRequestInProgress(null);
			recordingSession.setRequestDoc(null);
		}	
	}



	public static Document getMediaStreamFileResource(String fileNID, MRDB db, MRLog log, String userJID) throws Exception {
		 
		MRRequest request = MRRequest.createRequest(userJID);
		Document fileDoc = MRXMLDBUtils.readElement(request, log,db,fileNID);
		if (fileDoc == null)
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "<nid>"
					+ fileNID + "</nid>");

		return fileDoc;
	}


	
	
	public static void validateAction(Document notifyMessageDoc, String action,
			String nodeID, MRDB schema, MRLog log, String userJID, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
			//MonitoringSession.handleStreamMessages(notifyMessageDoc, action, nodeID, schema, log, proxyQueue, userJID, xmpp, resourceFactory, serviceAgentFactory);
		if(recordingSession == null) {
			return;
		}
		else {
			recordingSession.handleEvent(notifyMessageDoc, action, nodeID, schema, log, userJID, xmpp, resourceFactory, serviceAgentFactory);
		}
	}

	public static void validatePauseResumeRecordingAgentResponseMessage(ProxyClient proxyClient, 
			MRLog log, MRDB schema, String userJID, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, 
			Document notifyMessageDoc, String proxyResponseType,
			String proxyRequestType) throws MRException, Exception {
		String responseStatus = MRXMLUtils.getValueXpath(notifyMessageDoc,
				"//status", "");
		if (Utils.getIntValue(responseStatus, MRLog.OK) != MRLog.OK) {
			generateProxyFailureMessage(Utils.getIntValue(
					responseStatus, ProxyExceptionCodes.OTHER),  MRXMLUtils
					.getValueXpath(notifyMessageDoc, "//Description",
					"None"),proxyResponseType, userJID, proxyClient , log, schema, xmpp, resourceFactory,  serviceAgentFactory);
		}
	}

	public static void validateStartRecordingAgentResponseMessage(ProxyClient proxyClient, MRLog log,
			MRDB schema, String userJID, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document notifyMessageDoc, String actionResponse, String streamNID,
			String responseStatus) throws MRException, Exception {
		generateProxyFailureMessage(Utils.getIntValue(
				responseStatus, ProxyExceptionCodes.OTHER),  MRXMLUtils
				.getValueXpath(notifyMessageDoc, "//Description",
				"None"),ProxyConstants.STARTRECORDINGRESPONSE, userJID, proxyClient , log, schema, xmpp, resourceFactory,  serviceAgentFactory);
	}

	public static StringTokenizer generateProxyRecordResponseMessage(
			String message, String proxyResponseType, String proxyRequestType,
			double[] diskSpaceArray, MRLog log, MRDB schema, String userJID, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		StringTokenizer st = new StringTokenizer(message,
				ProxyConstants.SEPERATOR);
		String fileNID = null;
		String recordID = null;
		String strcode = null;
		String description = null;
		
		Document requestDoc = null;

		
		if (st.hasMoreTokens()) {
			fileNID = st.nextToken();
		}

		if(fileNID!= null && fileNID.trim().equalsIgnoreCase("None")) {
			fileNID = null;
		}
		if (st.hasMoreTokens()) {
			recordID = st.nextToken();
		}

		if (st.hasMoreTokens()) {
			strcode = st.nextToken();
		}

		if (st.hasMoreTokens()) {
			description = st.nextToken();
		}
		
		if (st.hasMoreTokens()) {
			requestDoc = MRXMLUtils.stringToDocument(st.nextToken());
		}
	
		
		int code = Utils.getIntValue(strcode, MRLog.OK);

		try {
			if ((code != MRLog.OK)) {

					if (fileNID != null)
					{
										
					if (proxyRequestType.equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)) {
						recordingSession.setStreamState(MediaRoomServiceConst.STREAM_STOPPED);
					}
					return generateProxyRecordResponseMessage(
							proxyResponseType, proxyRequestType, fileNID,diskSpaceArray,log,
							schema, userJID, db, xmpp, resourceFactory,
							serviceAgentFactory, code, description, requestDoc);
				}

				return generateProxyRecordResponseFailedMessage(recordID, null,
						 proxyResponseType, proxyRequestType, code,
						description, diskSpaceArray, requestDoc,  log,  schema,  userJID, xmpp,
						 resourceFactory,
						 serviceAgentFactory);
			}

			if(recordingSession.getProxyRequestInProgress() == null) {
				return null;
			}

			return generateProxyRecordResponseMessage(proxyResponseType, proxyRequestType, fileNID,diskSpaceArray,log, schema,
					userJID, db, xmpp, resourceFactory, serviceAgentFactory,
					ProxyExceptionCodes.NOERROR, "NoError", requestDoc);
		} catch (MRException exp) {
			exp.printStackTrace();
			log
			.addInfo("Exception Occur While Generating the proxy response Message "
					+ exp.getData());
			ProxyException proxyException = ProxyClientAppUtils.generateProxyException(exp.getCode(), exp.getData());
			return generateProxyRecordResponseFailedMessage(recordID, fileNID,
					proxyResponseType, proxyRequestType, proxyException.getCode(), proxyException.getData(), diskSpaceArray, requestDoc,  log,  schema,  userJID,   xmpp,
					 resourceFactory,
					 serviceAgentFactory);
		} catch (Exception exp) {
			exp.printStackTrace();
			log
			.addInfo("Exception Occur While Generating the proxy response Message "
					+ exp.toString());
			return generateProxyRecordResponseFailedMessage(recordID, fileNID,
					proxyResponseType, proxyRequestType,
					ProxyExceptionCodes.OTHER, ProxyExceptionCodes
					.getErrorDescription(ProxyExceptionCodes.OTHER), diskSpaceArray, requestDoc,
					log,   schema,  userJID,  xmpp,
					 resourceFactory,
					 serviceAgentFactory);
		}
	}

	
	public static StringTokenizer generateBookMarkResponseAndMediaTimestampResponse(
			String responseMessage, Document requestDoc, MRLog log)
	throws MRException, Exception {

		
		
		String requestID = "None";
		String agentName = "None";
		String requestorJID = "None";
		String requestornickName = "None";
		String recordID = "None";
		
		String proxyRequestType = requestDoc.getRootElement().getName();
		String proxyResponseType = MRRequest.getResponseName(proxyRequestType);
		
		if(requestDoc != null) {
			
			requestID = MRXMLUtils.getValueXpath(requestDoc,
					"//requestID", "None");
			requestID = (((requestID == null) || (requestID.length() == 0)) ? "None"
					: requestID);

			agentName = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/agentName", "None");
			agentName = (((agentName == null) || (agentName.length() == 0)) ? "None"
					: agentName);

			requestorJID = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/JID", "None");
			requestorJID = (((requestorJID == null) || (requestorJID.length() == 0)) ? "None"
					: requestorJID);

			requestornickName = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/nickName", "None");
			requestornickName = (((requestornickName == null) || (requestornickName
					.length() == 0)) ? "None" : requestornickName);
			recordID = MRXMLUtils.getValueXpath(requestDoc,
					"//ID", "None");
			recordID = (((recordID == null) || (recordID
					.length() == 0)) ? "None" : recordID);
			
		}
		
		StringBuffer proxyMessage = new StringBuffer("send "
				+ ProxyConstants.SEPERATOR + proxyResponseType
				+ ProxyConstants.SEPERATOR + requestID + ProxyConstants.SEPERATOR + 
				agentName + ProxyConstants.SEPERATOR + 
				requestorJID + ProxyConstants.SEPERATOR
				+ requestornickName + ProxyConstants.SEPERATOR			
				+ recordID + ProxyConstants.SEPERATOR + 
				responseMessage);

		log.addInfo("Sending Proxy " + proxyResponseType + " Response Message",
				MRLog.DEBUG, MRLog.NOTIFY);
		//deleteProxyRequestMessageFromRecordingSession(proxyRequestType, fileNID);

		return new StringTokenizer(proxyMessage.toString(),
				ProxyConstants.SEPERATOR);

	}
	
	
	public static StringTokenizer generateProxyRecordResponseFailedMessage(
			String recordID, String fileNID,
			String proxyResponseType, String proxyRequestType, int code,
			String description, double[] diskSpaceArray, Document requestDoc, MRLog log, MRDB schema, String userJID, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) {
		try {
			String requestID = "None";
			String agentName = "None";
			String requestorJID = "None";
			String requestornickName = "None";
			
			if(requestDoc == null && recordingSession!=null) {
				requestDoc = recordingSession.getRequestDoc();
			}
			if(requestDoc != null) {
				
				requestID = MRXMLUtils.getValueXpath(requestDoc,
						"//requestID", "None");
				requestID = (((requestID == null) || (requestID.length() == 0)) ? "None"
						: requestID);

				agentName = MRXMLUtils.getValueXpath(requestDoc,
						"//requestorInfo/agentName", "None");
				agentName = (((agentName == null) || (agentName.length() == 0)) ? "None"
						: agentName);

				requestorJID = MRXMLUtils.getValueXpath(requestDoc,
						"//requestorInfo/JID", "None");
				requestorJID = (((requestorJID == null) || (requestorJID.length() == 0)) ? "None"
						: requestorJID);

				requestornickName = MRXMLUtils.getValueXpath(requestDoc,
						"//requestorInfo/nickName", "None");
				requestornickName = (((requestornickName == null) || (requestornickName
						.length() == 0)) ? "None" : requestornickName);
				
			}
			
			
			StringBuffer proxyMessage = new StringBuffer("send "
					+ ProxyConstants.SEPERATOR + proxyResponseType
					+ ProxyConstants.SEPERATOR);
			proxyMessage.append(generateProxyRecordResponseMessage(proxyRequestType,
					requestID, agentName, requestorJID, requestornickName,
					recordID, "0", "0", "None", "None", String.valueOf(diskSpaceArray[0]), String.valueOf(diskSpaceArray[1]), String
					.valueOf(code), description, "None", "None",
					"None", "None", "0"));

			log.addInfo("Sending Proxy " + proxyResponseType
					+ " Failure Response Message", MRLog.DEBUG, MRLog.NOTIFY);
			/*if ((proxyRequestType
					.equalsIgnoreCase(ProxyConstants.STARTRECORDINGREQUEST)
					|| proxyRequestType
					.equalsIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST)) && recordingSession != null && recordID.equalsIgnoreCase(recordingSession.getRecordID())) {
				
					deleteRecordingSession(recordID, userJID, log,  schema,  xmpp,  resourceFactory,  serviceAgentFactory);
					
			}*/
			return new StringTokenizer(proxyMessage.toString(),
					ProxyConstants.SEPERATOR);
		} catch (Exception exp) {
			exp.printStackTrace();

			return null;
		}
	}
	
	public static String getState(Element mediaStreamFileResource) throws Exception {
		String fileNID = null;
		if(mediaStreamFileResource!=null) {
			fileNID = MRXMLUtils.getAttributeValue(mediaStreamFileResource,"NID");
		}
		String recordStatus = MRXMLUtils.getValueXpath(mediaStreamFileResource,"//State/StateFlag", "None");
		recordStatus = (((recordStatus == null) || (recordStatus.length() == 0)) ? "None"
				: recordStatus);
		if(recordStatus.equalsIgnoreCase(MediaStoreServiceConst.BUSY))		{//Busy
			recordStatus = getFileStatusFromRecordingSession(fileNID);
			if(recordStatus.equalsIgnoreCase("None")) {
				recordStatus = MediaStoreServiceConst.RECORDING_STARTED;
			}
			
		}
		if(recordStatus.equalsIgnoreCase(MediaStoreServiceConst.READY) || recordStatus.equalsIgnoreCase(MediaStoreServiceConst.ERROR) || recordStatus.equalsIgnoreCase(MediaStoreServiceConst.NOT_READY) || recordStatus.equalsIgnoreCase(MediaStoreServiceConst.CREATED)) {
			recordStatus = MediaStoreServiceConst.RECORDING_STOPPED;
		}
		return recordStatus;
	}

	public static StringTokenizer generateProxyRecordResponseMessage(String proxyResponseType,String proxyRequestType, String fileNID, double[] diskSpaceArr, MRLog log, MRDB schema,
			String userJID, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, int code,
			String description, Document requestDoc) throws MRException, Exception {

		Document mediaFileDoc = ProxyClientAppUtils.getMediaStreamFileResource(fileNID, db, log, userJID);
		String startTC = "0.0";
		String endTC = "0.0";
	
		

		String requestID = "None";
		String agentName = "None";
		String requestorJID = "None";
		String requestornickName = "None";
		
		if(requestDoc == null && recordingSession!=null) {
			requestDoc = recordingSession.getRequestDoc();
		}
		if(requestDoc != null) {
			
			requestID = MRXMLUtils.getValueXpath(requestDoc,
					"//requestID", "None");
			requestID = (((requestID == null) || (requestID.length() == 0)) ? "None"
					: requestID);

			agentName = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/agentName", "None");
			agentName = (((agentName == null) || (agentName.length() == 0)) ? "None"
					: agentName);

			requestorJID = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/JID", "None");
			requestorJID = (((requestorJID == null) || (requestorJID.length() == 0)) ? "None"
					: requestorJID);

			requestornickName = MRXMLUtils.getValueXpath(requestDoc,
					"//requestorInfo/nickName", "None");
			requestornickName = (((requestornickName == null) || (requestornickName
					.length() == 0)) ? "None" : requestornickName);
			
		}
		
		String recordID = MRXMLUtils.getValueXpath(mediaFileDoc,
				"//MetaData/Tag[@Name='ID']", "");
		String date = MRXMLUtils.getValueXpath(mediaFileDoc,
				"//Properties/DateCreated", "None");
		date = (new Date(Utils.getLongValue(date, new Date().getTime())))
		.toString();

		
		String recordStatus = getState(mediaFileDoc.getRootElement());
		if(mediaFileDoc != null) {
			startTC = getTimeCode(mediaFileDoc.getRootElement(),true);
			if(recordStatus.equalsIgnoreCase(MediaStoreServiceConst.RECORDING_STARTED)) {
				endTC = "-1";
			}
			else {
				endTC = getTimeCode(mediaFileDoc.getRootElement(),false);
			}	
		}	
		
		
		String patient = null;
		String procedureType = null;
		String physician = null;
		String notes = null;
		String timeOffset = null;

		if (ProxyConstants.RECORDINGSTATUSREQUEST
				.equalsIgnoreCase(proxyRequestType)) {
			patient = MRXMLUtils.getValueXpath(mediaFileDoc,
					"//MetaData/Tag[@Name='Patient']", "");
			procedureType = MRXMLUtils.getValueXpath(mediaFileDoc,
					"//MetaData/Tag[@Name='ProcedureType']", "");
			physician = MRXMLUtils.getValueXpath(mediaFileDoc,
					"//MetaData/Tag[@Name='Physician']", "");
			notes = MRXMLUtils.getValueXpath(mediaFileDoc,
					"//MetaData/Tag[@Name='Notes']", "");
			timeOffset = MRXMLUtils.getValueXpath(mediaFileDoc,
					"//MetaData/Tag[@Name='StartTimeOffset']", "");
		}

		String diskSpaceUsed = null;
		String totalDiskSpaceAvailable = null;

		if (diskSpaceArr == null) {
			diskSpaceUsed = String.valueOf(Double.valueOf(0));
			totalDiskSpaceAvailable = String.valueOf(Double.valueOf(0));
		} else {
			diskSpaceUsed = String.valueOf(diskSpaceArr[0]);
			totalDiskSpaceAvailable = String.valueOf(diskSpaceArr[1]);
		}

		StringBuffer proxyMessage = new StringBuffer("send "
				+ ProxyConstants.SEPERATOR + proxyResponseType
				+ ProxyConstants.SEPERATOR);
		proxyMessage.append(generateProxyRecordResponseMessage(proxyRequestType,
				requestID, agentName, requestorJID, requestornickName,
				recordID, startTC, endTC, date, recordStatus,
				diskSpaceUsed, totalDiskSpaceAvailable, String.valueOf(code),
				description, patient, procedureType, physician, notes,
				timeOffset));
		log.addInfo("Sending Proxy " + proxyResponseType + " Message", MRLog.DEBUG,
				MRLog.NOTIFY);
		deleteProxyRequestMessageFromRecordingSession(proxyRequestType, fileNID);

		/*if (recordStatus
				.equalsIgnoreCase(MediaStoreServiceConst.RECORDING_STOPPED)
				|| recordStatus.equalsIgnoreCase("None")) {
			deleteRecordingSession(fileNID,  userJID,  log,  schema,  xmpp,  resourceFactory,  serviceAgentFactory);
		}*/
		return new StringTokenizer(proxyMessage.toString(),
				ProxyConstants.SEPERATOR);

	}

	public static String generateProxyRecordResponseMessage(
			String proxyRequestType, String requestID, String agentName,
			String user, String nickName, String recordID, String startTime,
			String endTime, String date, String status, String diskSpaceUsed,
			String totalDiskSpaceAvailable, String errorCode,
			String errorDescription, String patient, String procedureType,
			String physician, String notes, String timeOffset)
	throws MRException, Exception {
		StringBuffer proxyMessage = new StringBuffer();
		String seperator = ProxyConstants.SEPERATOR;

		proxyMessage.append(requestID + seperator); // Request ID
		proxyMessage.append(agentName + seperator); // Agent Name
		proxyMessage.append(user + seperator); // Requested User Name
		proxyMessage.append(nickName + seperator); // Request User Nick Name
		proxyMessage.append(recordID + seperator); // Recording ID
		proxyMessage.append(startTime + seperator); // Recording Start Time
		proxyMessage.append(endTime + seperator); // Recording End Time
		proxyMessage.append(date + seperator); // Data
		proxyMessage.append(status + seperator); // Recording Status
		proxyMessage.append(diskSpaceUsed + seperator); // Total Disk Space Used
		proxyMessage.append(totalDiskSpaceAvailable + seperator); // Total disk
		// space
		// Available
		proxyMessage.append(errorCode + seperator); // Error Code
		proxyMessage.append(errorDescription + seperator); // Error Description

		if (ProxyConstants.RECORDINGSTATUSREQUEST
				.equalsIgnoreCase(proxyRequestType)) {
			proxyMessage.append(timeOffset + seperator); // time offset
			proxyMessage.append(patient + seperator); // Patient name
			proxyMessage.append(procedureType + seperator); // Procedure Type
			proxyMessage.append(physician + seperator); // Physician name
			proxyMessage.append(notes + seperator); // notes
		}

		return proxyMessage.toString();
	}

	public static String getFileStatus(String fileNID, MRDB db, MRLog log, String userJID) throws MRException,
	Exception {
		Document mediaFileDoc = ProxyClientAppUtils.getMediaStreamFileResource(fileNID, db, log, userJID);

		if (mediaFileDoc == null) {
			return MediaStoreServiceConst.RECORDING_STOPPED;
		}

		String fileStatus = MRXMLUtils.getValueXpath(mediaFileDoc,
				"//State/StateFlag", "");

		return fileStatus;
	}
	
	public static String getFileStatusFromRecordingSession(String fileNID) throws MRException,
	Exception {

		String fileState = recordingSession.getFileState();
		if(fileState == null || fileState.equals("") || fileState.trim().equalsIgnoreCase("None")) {
			return MediaStoreServiceConst.RECORDING_STOPPED;
		}
		return fileState;
	}

	public static String getRecordID(Document fileDoc) throws Exception {
		String xPath = "//MetaData/Tag[@Name='ID']";
		return MRXMLUtils.getValueXpath(fileDoc, xPath, "");
	}

	public static String getProxyFileStatus(String fileNID, MRDB schema, MRLog log,
			String userJID) throws MRException, Exception {
		String fileStatus = null;

		MRRequest request = MRRequest.createRequest(userJID);
		Document mediaFileDoc = null;

		mediaFileDoc = MRXMLDBUtils.readElement(request, log, schema, fileNID);
		if (mediaFileDoc == null)
			throw new MRException(MRExceptionCodes.NODENOTFOUND, "<nid>"
					+ fileNID + "</nid>");

		fileStatus = MRXMLUtils.getValueXpath(mediaFileDoc,
				"//State/StateFlag", "None");
		log.addInfo("FileStatus=" + fileStatus, MRLog.DEBUG, MRLog.NOTIFY);
		if ((fileStatus == null) || (fileStatus.length() == 0)) {
			return null;
		}

		switch (RecordingStaus.valueOf(fileStatus)) {
		
		case Busy:
			//System.out.println("File is in Busy State");
			break;
		case Ready:
			//System.out.println("File is in Ready State");
			 	
			if(recordingSession.getProxyRequestInProgress() == null || !recordingSession.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST) || recordingSession.getSessionState() != ProxySession.WAIT_FOR_PUB_SUB_EVENT){
				return null;
			}

			fileStatus = ProxyConstants.RECORD_STOPPED;

			break;
		case Error:
			if(recordingSession.getProxyRequestInProgress() == null || !recordingSession.getProxyRequestInProgress().equalsIgnoreCase(ProxyConstants.STOPRECORDINGREQUEST) || recordingSession.getSessionState() != ProxySession.WAIT_FOR_PUB_SUB_EVENT){
				return null;
			}

			fileStatus = MediaStoreServiceConst.ERROR;

			break;
		default:
			return null;
		}

		return fileStatus;
	}

	// Get the enable KBM value from stream data document -- added by Padmaja
	public static String getEnableKBM(Document streamDataDoc)
	throws MRException, Exception {
		String enableKBM = MRXMLUtils.getValueXpath(streamDataDoc,
				"//EnableKbm", "");

		if (MediaRoomServiceConst.TRUE.equalsIgnoreCase(enableKBM)) {
			enableKBM = "yes";
		} else {
			enableKBM = "no";
		}

		return enableKBM;
	}

	public static void sendProxyResponseMessage(String responseType, String responseMessage, String userJID, ProxyClient proxyClient, double[] diskSpaceArr, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {

		try {
			
			int errorCode = ProxyExceptionCodes.NOERROR;
			String recordID = "None";
			if(responseType.equalsIgnoreCase(ProxyConstants.STOPRECORDINGRESPONSE) || responseType.equalsIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) || responseType.equalsIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE)){
				StringTokenizer st = new StringTokenizer(responseMessage, ProxyConstants.SEPERATOR);
				if (st.hasMoreTokens()) {
					st.nextToken();
					recordID  = st.nextToken();
					errorCode = Integer.valueOf(st.nextToken());
				}
			}	
			StringTokenizer proxyResponse = ProxyClientAppUtils.generateResponse(responseMessage, responseType, userJID,diskSpaceArr,log, db, xmpp, resourceFactory, serviceAgentFactory);
			if(proxyResponse != null) {
				proxyClient.sendResponse(proxyResponse);
				if(
						(responseType.equalsIgnoreCase(ProxyConstants.STOPRECORDINGRESPONSE) && recordID.equalsIgnoreCase(recordingSession.getRecordID()))
						|| (responseType.equalsIgnoreCase(ProxyConstants.RECORDINGSTATUSRESPONSE) && errorCode == ProxyExceptionCodes.USER_LOGGED_OFF) 
						|| (responseType.equalsIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE) && errorCode!= ProxyExceptionCodes.NOERROR && recordID.equalsIgnoreCase(recordingSession.getRecordID()))
						
				 ) {
					deleteRecordingSession(userJID, log, db,  xmpp,  resourceFactory,  serviceAgentFactory);
				}
				
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	
	public static void sendProxyResponseMessage(String responseType, String responseMessage, String userJID, ProxyClient proxyClient, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
		//System.out.println(message);
		double diskSpaceArr[] = {0,0};
		sendProxyResponseMessage(responseType, responseMessage, userJID, proxyClient, diskSpaceArr, log, db, xmpp,  resourceFactory, serviceAgentFactory);
	}

	public static String generateProxyStream(Document streamDataDoc, Document streamStatusDoc, int state,MRLog log, MRDB schema) {
		try {
			log.addInfo("Send Proxy Auth Notify Message", MRLog.DEBUG,
					MRLog.NOTIFY);

			String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc,
					"//destAgentJID", "");
			String enableKBM = getEnableKBM(streamDataDoc);
			String startTime = MRXMLUtils.getValueXpath(streamDataDoc,
					"//startTime", "0");
			String sourceAgentJID = MRXMLUtils.getValueXpath(streamDataDoc,
					"//sourceAgentJID", "");
			
			String connect = MediaRoomServiceConst.YES;
			if(state != 1) {
				connect = MediaRoomServiceConst.NO;
			}
			
			String proxyMessage = generateProxyAuthNotifyMessage(sourceAgentJID,
					destAgentJID, connect, enableKBM, "" + state, startTime,
					schema, log);
			return proxyMessage;
		}catch(Exception e) {
			e.printStackTrace();
			return null;
		}	
	}

	public static String generateProxyAuthNotifyMessage(
			Document streamDataDoc, Document streamStatusDoc, int state,
			MRLog log, MRDB schema) {
		try {
			String proxyMessage = generateProxyStream( streamDataDoc,  streamStatusDoc,  state, log,  schema);
			String id = Long.toHexString(ProxyConstants.wheel.nextLong());
			StringBuffer proxyMess = new StringBuffer("send"
					+ ProxyConstants.SEPERATOR + "authorizestreamnotification"
					+ ProxyConstants.SEPERATOR + id + ProxyConstants.SEPERATOR);
			proxyMess.append(proxyMessage);
			return proxyMess.toString();

		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
		
	}
	
	public static String generateProxyAuthNotifyMessage(String sourceAgentJID,
			String destAgentJID, String connect, String remote, String state,
			String startTime, MRDB schema, MRLog log) throws MRException,
			Exception {
		StringBuffer proxyMessage = new StringBuffer();
		String seperator = ProxyConstants.SEPERATOR;
		String sourceAgentName = ServiceAgentUtils.getAgentName(sourceAgentJID);
		proxyMessage.append(sourceAgentName + seperator); // Source Agent Name
		proxyMessage.append(sourceAgentJID + seperator); // Source Agent JID

		String sourceAgentNickName = getNickName(sourceAgentJID, schema, log);
		proxyMessage.append(sourceAgentNickName + seperator); // Agent Nick Name

		String destAgentName = ServiceAgentUtils.getAgentName(destAgentJID);
		proxyMessage.append(destAgentName + seperator); // Agent Name
		proxyMessage.append(destAgentJID + seperator); // Agent JID

		String destAgentNickName = getNickName(destAgentJID, schema, log);
		proxyMessage.append(destAgentNickName + seperator); // Agent Nick Name
		proxyMessage.append(connect + seperator); // Connect
		proxyMessage.append(remote + seperator); // Remote
		proxyMessage.append(state + seperator); // State
		proxyMessage.append(startTime + seperator); // Start time

		return proxyMessage.toString();
	}

	public static String generateProxyAuthRequestMessage(String sourceAgentJID,
			String destAgentJID, String connect, String remote, MRDB schema,
			MRLog log) throws MRException, Exception {
		StringBuffer proxyMessage = new StringBuffer();
		String seperator = ProxyConstants.SEPERATOR;
		String sourceAgentName = ServiceAgentUtils.getAgentName(sourceAgentJID);
		proxyMessage.append(sourceAgentName + seperator); // Source Agent Name
		proxyMessage.append(sourceAgentJID + seperator); // Source Agent JID

		String sourceAgentNickName = getNickName(sourceAgentJID, schema, log);
		proxyMessage.append(sourceAgentNickName + seperator); // Agent Nick Name

		String destAgentName = ServiceAgentUtils.getAgentName(destAgentJID);
		proxyMessage.append(destAgentName + seperator); // Agent Name
		proxyMessage.append(destAgentJID + seperator); // Agent JID

		String destAgentNickName = getNickName(destAgentJID, schema, log);
		proxyMessage.append(destAgentNickName + seperator); // Agent Nick Name
		proxyMessage.append(connect + seperator); // Connect
		proxyMessage.append(remote + seperator); // Remote

		return proxyMessage.toString();
	}
	
	
	public static void sendDeviceStatusRequest(String storeAgentJID, String clientData, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String userJID)
	throws MRException, Exception {

		String clientdataString = "";
		if(clientData != null) {
			clientdataString = URLEncoder.encode(clientData, "UTF-8");
		}	
		String agentName = ServiceAgentUtils.getAgentName(
				MRServiceConst.DEVICEADMINCLIENTAGENT, userJID);
		if (storeAgentJID == null) {
			return;
		}

		Document doc = MRXMLUtils.loadXMLResource(
				"/com/ipvs/systemservice/xml/DeviceStatusRequest.xml", log);
		MRXMLUtils.setValueXpath(doc, "//agentJID", storeAgentJID);
		MRXMLUtils.setValueXpath(doc, "//agent",
				MRServiceConst.DEVICEADMINSERVERAGENT);
		MRXMLUtils.setValueXpath(doc, "//RequestXPath",
		"//StorageElementsStatus");
		Element clientDataEL = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientDataEL.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(doc, "//header");
		MRXMLUtils.removeChild(headerElement,"clientdata");
		headerElement.add(clientDataEL);

		String requestNID = "DeviceStatusRequest" + "_" + userJID;
		MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO,
				requestNID, "systemservice", "DeviceStatusRequest",
				new MRMessage(doc), null);

		ServiceAgentI serviceAgent = serviceAgentFactory
		.getServiceAgent(agentName);
		DeviceAdminClientAgent agent = (DeviceAdminClientAgent) serviceAgent;
		
		if (agent != null) {
			agent.sendGetDeviceStatusRequest(storeAgentJID,request, xmpp, log, db, resourceFactory,serviceAgentFactory);

		}

		return;
	}
	public static void processResponse(Document responseDoc,
			ProxyClient proxyClient, MRDB db, MRLog log, String userJID,
			XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
			
			String requestName = MRXMLUtils.getValueXpath(responseDoc,
					"//requestname", "");
			//String recordID = null;
			
			switch (RequestNameList.valueOf(requestName)) {

			case UpdateDestFileMetaDataRequest:
				log.addInfo(": Received UpdateDestFileMetaDataResponse ", MRLog.INFO,MRLog.NOTIFY);
				Element fileEL = MRXMLUtils.getElementXpath(responseDoc, "//"+ AMConst.MEDIA_STREAM_FILE_RESOURCE);
				if(fileEL!=null) {
					//recordID = MRXMLUtils.getValueXpath(fileDoc,"//MetaData/Tag[@Name='ID']", "");
				}	
				ProxyClientAppUtils.handleRecordResponseMessages(responseDoc, proxyClient, userJID, db,
						log, xmpp, resourceFactory, serviceAgentFactory, ProxyConstants.UPDATERECORDINGREQUEST,
						ProxyConstants.UPDATERECORDINGRESPONSE);

				break;
			case GetNewBookmarkRequest:
				ProxyClientAppUtils.handleGetNewBookmarkResponse(responseDoc, proxyClient,  userJID,  xmpp,  log,  db,  resourceFactory,  serviceAgentFactory);
				break;
			case AddBookmarkRequest:
				log.addInfo(": Received AddBookmarkResponse ", MRLog.INFO,MRLog.NOTIFY);
				ProxyClientAppUtils.handleAddBookmarkResponse(responseDoc, proxyClient, log);
				break;

			case GetSrcMediaFileListRequest:
				log.addInfo(": Received GetSrcMediaFileListResponse ",MRLog.INFO,MRLog.NOTIFY);
				fileEL = MRXMLUtils.getElementXpath(responseDoc, "//"+ AMConst.MEDIA_STREAM_FILE_RESOURCE);
					ProxyClientAppUtils.handleRecordResponseMessages(responseDoc, proxyClient, userJID, db,
							log, xmpp, resourceFactory, serviceAgentFactory, ProxyConstants.RECORDINGSTATUSREQUEST,
							ProxyConstants.RECORDINGSTATUSRESPONSE);
				break;
			case GetDestMediaDirListRequest:
				log.addInfo(": Received GetDestMediaDirListResponse ", MRLog.INFO,MRLog.NOTIFY);
				String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
				clientData = URLDecoder.decode(clientData, "UTF-8");
				StringTokenizer st = new StringTokenizer(clientData, ProxyConstants.SEPERATOR);
				String responseType = null;
				if (st!= null && st.hasMoreTokens()) {
					responseType = st.nextToken();
				}
				
				Element medisStreamDirResourceEL = MRXMLUtils.getElementXpath(responseDoc, "//MediaStreamDirResource");
				if(medisStreamDirResourceEL != null) {
					Document mediaStreamDirResource = MRXMLUtils.elementToDocument(medisStreamDirResourceEL);
					Element mediaStorePortResourceNIDElement = null;
					try {
						mediaStorePortResourceNIDElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaStreamDirResource,true, db, log);
					} catch(Exception e) {
					e.printStackTrace();
					}
					if(mediaStorePortResourceNIDElement != null) {
						String mediaStorePortResourceNID = MRXMLUtils.getValue(mediaStorePortResourceNIDElement);
						String storeOwnerJID = MediaRoomServiceUtils.getContactJIDForPort(MRRequest.createRequest(userJID), log, db,mediaStorePortResourceNID);
						String dirDeviceElementName = MRXMLUtils.getAttributeValue(mediaStorePortResourceNIDElement,"deviceElementId");
						clientData = dirDeviceElementName + ProxyConstants.SEPERATOR + clientData;
						ProxyClientAppUtils.sendDeviceStatusRequest(storeOwnerJID,clientData, xmpp, log, db, resourceFactory, serviceAgentFactory,userJID);
					}
					else {
						ProxyClientAppUtils.sendProxyResponseMessage(responseType,  clientData.replace(responseType + ProxyConstants.SEPERATOR, ""),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);	
					}
				}
				else {
					ProxyClientAppUtils.sendProxyResponseMessage(responseType,  clientData.replace(responseType + ProxyConstants.SEPERATOR, ""),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);
				}
				break;
			case SetServiceDomainRequest:
				break;
	
			case DeviceStatusRequest:
				log.addInfo(": Received DeviceStatusResponse ", MRLog.INFO,MRLog.NOTIFY);
				String dirDeviceElementName = null; 
				String clientdata = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
				clientdata = URLDecoder.decode(clientdata, "UTF-8");
				st = new StringTokenizer(clientdata, ProxyConstants.SEPERATOR);
				if(st != null && st.hasMoreTokens()) {
					dirDeviceElementName = st.nextToken();
					responseType = st.nextToken();
					clientdata = clientdata.replace(dirDeviceElementName + ProxyConstants.SEPERATOR + responseType + ProxyConstants.SEPERATOR, "");
					if(responseType!= null) {
						double[] diskSpaceArr = getDeviceDiskSpace(responseDoc, dirDeviceElementName); 
						ProxyClientAppUtils.sendProxyResponseMessage(responseType,  clientdata,  userJID, proxyClient, diskSpaceArr, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);
					}
				}	
				break;
			case CreateConnectionRequest:
				if(recordingSession == null || ProxySession.STATE_DONE == recordingSession.getSessionState()) {
					String roomNID = MRXMLUtils.getValueXpath(responseDoc, "//roomNID","");
					if(roomNID!=null && !"".equalsIgnoreCase(roomNID)) {
						ProxyClientAppUtils.deleteMediaRoom(roomNID, MRRequest.createRequest(userJID), log, db, xmpp, resourceFactory, serviceAgentFactory);
					}	
					return;
				} 
				recordingSession.handleTransition(responseDoc, db, log, userJID, xmpp, resourceFactory, serviceAgentFactory);
				break;
				
			case GetServerTimeRequest:
				Element clientDataEL = MRXMLUtils.getElementXpath(responseDoc, "//"+MRConst.CLIENT_DATA);
				Element requestElement = MRXMLUtils.getFirstChild(clientDataEL);
				if(ProxyConstants.UPDATERECORDINGREQUEST.equalsIgnoreCase(requestElement.getName())) {
					long serverTime = 0;
					try {
						serverTime = Long.valueOf(MRXMLUtils.getValueXpath(responseDoc, "//serverTime", ""));
					}catch(Exception e) {
						e.printStackTrace();
						log.addInfo(": Received Invalid Server Time ", MRLog.INFO,MRLog.NOTIFY);
						serverTime = System.currentTimeMillis();
					}
					String recordID = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(requestElement), "//ID", "");
					MRRequest request = MRRequest.createRequest(userJID);
					ProxyClientAppUtils.sendUpdateRecordingRequest(requestElement, recordID, serverTime, userJID, request, log, db, xmpp, serviceAgentFactory );
					return;
				} 
				
			default:
				if(recordingSession == null) {
					return;
				}
				recordingSession.handleTransition(responseDoc, db, log, userJID, xmpp, resourceFactory, serviceAgentFactory);
			}	
		
	}
	
	private static  String getSourceNID(String userJID,  MRDB db, MRLog log) throws MRException, Exception {
		String title = ProxyClientAppUtils.getSourceTitle();
		String[] whereXPath = { "//MediaStreamSrcPortResource/.[Info/Title = '" + title + "']" };

		MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE, log);
		String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
		Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE + "List");
		Element whereAssistXML = MRXMLUtils.newElement("whereAssistXML");

		MRXMLDBUtils.findChildElements(parentNID, responseEL, null, 1, whereAssistXML, whereXPath, MRRequest.createRequest(userJID), db, log);

		String mediaStreamSrcPortResourceNID =MRXMLUtils.getAttributeValue(MRXMLUtils.getChild( responseEL,"MediaStreamSrcPortResource"),"NID");

		return mediaStreamSrcPortResourceNID;
	}

	public static void sendGetDestDirListRequest(MRRequest request, String clientData, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	throws MRException, Exception {
		String sourceNID = getSourceNID(userJID, db, log);	
		String MEDIAROOM_XMLFILEPATH = "/com/ipvs/mediaroomservice/xml/";
		Document msgdoc = MRXMLUtils.loadXMLResource(MEDIAROOM_XMLFILEPATH + "GetDestMediaDirListRequest.xml", log);

		String clientdataString = URLEncoder.encode(clientData, "UTF-8");
		Element clientDataEL = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientDataEL.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(msgdoc, "//header");
		MRXMLUtils.removeChild(headerElement,"clientdata");
		headerElement.add(clientDataEL);
		//Element requestEl = msgdoc.getRootElement().element("data");
		MRXMLUtils.setValueXpath(msgdoc, "//sourceNID", sourceNID);
		MRXMLUtils.setValueXpath(msgdoc, "//SortOrder", "ascending");
		MRXMLUtils.setValueXpath(msgdoc, "//SortByXpath", "$VAR/min(//MediaStorePortResourceNIDList/MediaStorePortResourceNID[@priority!='']/@priority)");
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), msgdoc, log, "GetDestMediaDirListRequest", serviceAgentFactory);
	}
	
	public static double [] getDeviceDiskSpace (Document responseDoc, String dirDeviceElementName) {
		try {
		
		String parametersXPath = null;
			
		parametersXPath = "//Parameters[./Parameter/@name = 'Name' and ./Parameter = '" + dirDeviceElementName + "']";	
		Element diskParameters = MRXMLUtils.getElementXpath(responseDoc, parametersXPath);

		String usedDiskSpace = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(diskParameters),
				"//Parameter[@name='Used']",
		"");
		String availableDiskSpace = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(diskParameters),
				"//Parameter[@name='Total']",
		"");

		usedDiskSpace = (((usedDiskSpace == null) || (usedDiskSpace
				.length() == 0)) ? "0" : usedDiskSpace);
		availableDiskSpace = (((availableDiskSpace == null) || (availableDiskSpace
				.length() == 0)) ? "0" : availableDiskSpace);

		double[] diskSpaceArr = new double[2];
		diskSpaceArr[0] = Utils
		.getLongValue(usedDiskSpace, Long.valueOf(0));
		diskSpaceArr[1] = Utils.getLongValue(availableDiskSpace, Long
				.valueOf(0));
		return diskSpaceArr;

		}catch(Exception e) {
			e.printStackTrace();
			double[] diskSpaceArr = {0,0};
			return diskSpaceArr;
		}
	}
	
	public static void handleMediaRoomResponseMessages(Document responseDoc,
			String mediaRoomRequestName,
			ProxyClient proxyClient, MRLog log,
			MRDB schema, String userJID, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		Element exceptionEL = MRXMLUtils.getElementXpath(responseDoc,
		"//Exception");
		if (exceptionEL != null) {
			String proxyRequestType = recordingSession.getProxyRequestInProgress();
			String proxyResponseType = MRRequest.getResponseName(proxyRequestType);
			String strCode = MRXMLUtils
			.getValueXpath(responseDoc, "//Code", "");
			int code = Utils.getIntValue(strCode, Integer.valueOf(0));
			String description = MRXMLUtils.getValueXpath(responseDoc,
					"//Description", "");
			generateProxyFailureMessage(code, description,
					proxyResponseType, userJID, proxyClient , log, schema, xmpp, resourceFactory,  serviceAgentFactory);
			/*if (proxyResponseType
					.equalsIgnoreCase(ProxyConstants.STARTRECORDINGRESPONSE)) {
				recordingSession.setStreamNID(null);
				String roomNID = recordingSession.getRoomNID();
				sendStopRecordingRequest(roomNID, MRRequest.createRequest(userJID),
						log, schema, xmpp, resourceFactory, serviceAgentFactory);
			}*/
		}
	}
	
	
	public static void sendResponseMessage(Document proxyRequestDoc,
			Element mediaStreamFileResource, int errorCode,
			String errorDescription, String id, ProxyClient proxyClient, MRRequest request,
			String userJID, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
			String requestType = proxyRequestDoc.getRootElement().getName();
			String responseType = MRRequest.getResponseName(requestType);
			String clientDataString = MRXMLUtils.documentToString(proxyRequestDoc);
			String message = responseType  + ProxyConstants.SEPERATOR + "None" + ProxyConstants.SEPERATOR + id + ProxyConstants.SEPERATOR + errorCode + ProxyConstants.SEPERATOR + errorDescription + ProxyConstants.SEPERATOR + clientDataString + ProxyConstants.SEPERATOR;
			
			if(!ProxyConstants.MEDIATIMESTAMPREQUEST.equalsIgnoreCase(requestType) && !ProxyConstants.CREATEBOOKMARKREQUEST.equalsIgnoreCase(requestType)) {
				sendGetDestDirListRequest(MRRequest.createRequest(userJID), message, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);
			} else if (ProxyConstants.MEDIATIMESTAMPREQUEST.equalsIgnoreCase(requestType))  {
				String responseMessage = "0" + ProxyConstants.SEPERATOR;
				proxyClient.sendResponse(ProxyClientAppUtils.generateBookMarkResponseAndMediaTimestampResponse(responseMessage, proxyRequestDoc, log));
			} else if (ProxyConstants.CREATEBOOKMARKREQUEST.equalsIgnoreCase(requestType))  {
				String timestamp = MRXMLUtils.getValueXpath(proxyRequestDoc, "//Timestamp", "0");
				String notes = MRXMLUtils.getValueXpath(proxyRequestDoc, "//Notes", "");
				String responseMessage = timestamp + ProxyConstants.SEPERATOR + notes + ProxyConstants.SEPERATOR;
				proxyClient.sendResponse(ProxyClientAppUtils.generateBookMarkResponseAndMediaTimestampResponse(responseMessage, proxyRequestDoc, log));
			}
			
	}
	
	
	static void handleGetNewBookmarkResponse (Document responseDoc, 
	ProxyClient proxyClient, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,
	Exception {
		Element clientData = MRXMLUtils.getElementXpath(responseDoc, "//"+MRConst.CLIENT_DATA);
		String clientDataString = URLDecoder.decode(clientData.getText(), "UTF-8");
		Document clientDataDoc = null;
		clientDataDoc = MRXMLUtils.stringToDocument(clientDataString);
		String timestamp = MRXMLUtils.getValueXpath(responseDoc, "//AtTC", "0");
		if(ProxyConstants.MEDIATIMESTAMPREQUEST.equalsIgnoreCase(clientDataDoc.getRootElement().getName())) {
			String responseMessage = timestamp + ProxyConstants.SEPERATOR;
			proxyClient.sendResponse(ProxyClientAppUtils.generateBookMarkResponseAndMediaTimestampResponse(responseMessage, clientDataDoc, log));
		} else {
			MRXMLUtils.setValueXpath(clientDataDoc, "//Timestamp", timestamp);
			MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO,
					"1234", "serviceName", "CreateBookmarkRequest", new MRMessage(clientDataDoc), "");
			MRRequestHandlerI handler = new CreateBookmarkRequestHandler();
			handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		
	}

	static void handleAddBookmarkResponse (Document responseDoc, 
			ProxyClient proxyClient, MRLog log) throws MRException,
			Exception {
				Element clientData = MRXMLUtils.getElementXpath(responseDoc, "//"+MRConst.CLIENT_DATA);
				String clientDataString = URLDecoder.decode(clientData.getText(), "UTF-8");
				Document clientDataDoc = null;
				clientDataDoc = MRXMLUtils.stringToDocument(clientDataString);
				String timestamp = MRXMLUtils.getValueXpath(clientDataDoc, "//Timestamp", "0");
				String notes = MRXMLUtils.getValueXpath(clientDataDoc, "//Notes", "");
				String responseMessage = timestamp + ProxyConstants.SEPERATOR + notes + ProxyConstants.SEPERATOR;
				proxyClient.sendResponse(ProxyClientAppUtils.generateBookMarkResponseAndMediaTimestampResponse(responseMessage, clientDataDoc, log));
			}

	
	
	static void handleRecordResponseMessages(Document responseDoc, 
			ProxyClient proxyClient, String userJID, MRDB db, MRLog log, XMPPI xmpp, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String proxyRequestType, String proxyResponseType) throws MRException,
			Exception {
		String recordID = "None";
		String fileNID = "None";
		Element fileEL = null;
		Document fileDoc = null;
		//System.out.println(MRXMLUtils.documentToString(responseDoc));
		fileEL = MRXMLUtils.getElementXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
		Element clientData= null;
		String clientDataString = "";

		clientData = MRXMLUtils.getElementXpath(responseDoc, "//"+MRConst.CLIENT_DATA);
		clientDataString = URLDecoder.decode(MRXMLUtils.getValue(clientData), "UTF-8");
		Document clientDataDoc = null;
		if(ProxyConstants.UPDATERECORDINGREQUEST.equalsIgnoreCase(proxyRequestType)) {
			String tempClientDataString = "<clientdata>" + clientDataString + "</clientdata>";
			clientData = MRXMLUtils.stringToElement(tempClientDataString);
			clientDataString = MRXMLUtils.getValue(clientData);
			clientDataDoc = MRXMLUtils.stringToDocument(clientDataString);
		}
		else {
			clientDataDoc = MRXMLUtils.stringToDocument(clientDataString);
		}
		recordID = MRXMLUtils.getValueXpath(clientDataDoc,"//ID", "None");
		recordID = (((recordID == null) || (recordID.length() == 0)) ? "None": recordID);
		try {
			if (fileEL == null) {
				String responseType = MRRequest.getResponseName(proxyRequestType);
				String message = responseType  + ProxyConstants.SEPERATOR + "None" + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + ProxyExceptionCodes.ID_NOT_FOUND  + ProxyConstants.SEPERATOR + ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.ID_NOT_FOUND) + ProxyConstants.SEPERATOR + clientDataString + ProxyConstants.SEPERATOR;
				sendGetDestDirListRequest(MRRequest.createRequest(userJID), message, userJID, xmpp, log, db, resourceFactory, serviceAgentFactory);
				return;
			}
			fileDoc = MRXMLUtils.elementToDocument(fileEL);
			recordID = MRXMLUtils.getValueXpath(fileDoc,
					"//MetaData/Tag[@Name='ID']", "");
			fileNID = MRXMLUtils.getAttributeValue(fileEL,"NID");
			MRRequest request = MRRequest.createRequest(userJID);
			Element mediaStorePortResourceElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(fileDoc,true, request, db, log);
			String mediaStorePortResourceNID = MRXMLUtils.getValue(mediaStorePortResourceElement);
			String dirDeviceElementName = MRXMLUtils.getAttributeValue(mediaStorePortResourceElement,"deviceElementId");
			String storeOwnerJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortResourceNID);
			String message = dirDeviceElementName + ProxyConstants.SEPERATOR + proxyResponseType + ProxyConstants.SEPERATOR +  fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + ProxyExceptionCodes.NOERROR  + ProxyConstants.SEPERATOR + ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.NOERROR) + ProxyConstants.SEPERATOR + clientDataString + ProxyConstants.SEPERATOR;
			ProxyClientAppUtils.sendDeviceStatusRequest(storeOwnerJID, message, xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
				
		} catch (MRException exp) {
			exp.printStackTrace();
			ProxyException pe = ProxyClientAppUtils.generateProxyException(exp.getCode(), exp.getData());
			String proxyMessage  = fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR  + pe.getCode() + ProxyConstants.SEPERATOR + pe.getData();
			sendProxyResponseMessage(proxyResponseType,  proxyMessage.toString(),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);
		} catch (Exception exp) {
			exp.printStackTrace();
			String proxyMessage  = fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR  + ProxyExceptionCodes.OTHER + ProxyConstants.SEPERATOR + ProxyExceptionCodes.OTHER_DESCRIPTION;
			sendProxyResponseMessage(proxyResponseType,  proxyMessage.toString(),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);			
		}
	}

	public static void generateProxyFailureMessage(int code, String description, String proxyResponseType, String userJID, ProxyClient proxyClient, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
		String fileNID = null;
		String recordID = null;
		if(recordingSession.getFileNID()!= null) {
			fileNID = recordingSession.getFileNID();
		}
		else {
			fileNID = "None";
		}
		if(recordingSession.getRecordID()!=null) {
			recordID = recordingSession.getRecordID();
		}
		else {
			recordID = "None";
		}
		ProxyException pe = ProxyClientAppUtils.generateProxyException(code, description);
		//sendProxyResponseMessage(proxyResponseType, proxyQueue,	proxyMessage.toString(), log);
		try {
			if(recordingSession.getStoreOwnerJID()!= null) {
				String proxyMessage = recordingSession.getDirDeviceElementName() + ProxyConstants.SEPERATOR + proxyResponseType + ProxyConstants.SEPERATOR +  fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + pe.getCode()  + ProxyConstants.SEPERATOR + pe.getData() + ProxyConstants.SEPERATOR;
				ProxyClientAppUtils.sendDeviceStatusRequest(recordingSession.getStoreOwnerJID(), proxyMessage.toString(), xmpp, log, db, resourceFactory, serviceAgentFactory, userJID);
			}	
			else {
				String proxyMessage = fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + pe.getCode()  + ProxyConstants.SEPERATOR + pe.getData() + ProxyConstants.SEPERATOR;				
				sendProxyResponseMessage(proxyResponseType,  proxyMessage.toString(),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);				
			}
			
		} catch(Exception e) {
			String proxyMessage = fileNID + ProxyConstants.SEPERATOR + recordID + ProxyConstants.SEPERATOR + pe.getCode()  + ProxyConstants.SEPERATOR + pe.getData() + ProxyConstants.SEPERATOR;
			sendProxyResponseMessage(proxyResponseType,  proxyMessage.toString(),  userJID, proxyClient, log,  db,  xmpp,  resourceFactory, serviceAgentFactory);
		}	
	}

	
	
	
	public static enum MessageType {
		AgentMessage, AgentEvent, XMPPEvent;
	}

	public static enum ActionEventList {
		Ready, Busy, Error,StreamStartRequest, StreamStopped, StreamStarted, HandleXMPPEvent, RecordStarted, RecordPaused, RecordStopped, RecordResumed, StopOutput, StartOutput, NotifyResponse, UserPresence, StartRequest, Destroy, StreamStatusEvent;
	}

	public static enum RecordingStaus {
		Ready, Busy, Error, Recording, Stopped, Paused, Created;
	}

	public static enum RequestNameList {
		GetSrcMediaFileTrackRequest, AddDestFileRequest, CreateRecordingRequest, CreateConnectionRequest, UpdateDestFileMetaDataRequest, AddBookmarkRequest, GetSrcMediaFileListRequest, AddStreamActionRequest, AddTrackActionRequest, DeleteMediaRoomRequest, GetMediaRoomListRequest, DeviceStatusRequest, GetDestMediaDirListRequest, SetServiceDomainRequest, GetNewBookmarkRequest, GetServerTimeRequest;
	}

	public static enum ProxyRequestNameList {
		STARTRECORDINGREQUEST,
		STOPRECORDINGREQUEST,
		UPDATERECORDINGREQUEST,
		RESUMERECORDINGREQUEST,
		PAUSERECORDINGREQUEST,
		RECORDINGSTATUSREQUEST,
		CREATEBOOKMARKREQUEST,
		MEDIATIMESTAMPREQUEST,
		AUTHORIZATIONUPDATEREQUEST,
		GETSTREAMLISTREQUEST,
	
	}

		
	
	
	public static StringTokenizer generateResponse(String responseMessage,String responseType,  
			String userJID, double[] diskSpaceArray, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		if (responseType.compareToIgnoreCase(ProxyConstants.AUTHREQUEST) == 0) {
			return (new StringTokenizer(responseMessage,
					ProxyConstants.SEPERATOR));
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.LISTRESPONSE) == 0) {
			return (new StringTokenizer(responseMessage,
					ProxyConstants.SEPERATOR));
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.AUTHNOTIFY) == 0) {
			return (new StringTokenizer(responseMessage,
					ProxyConstants.SEPERATOR));
		}else if (responseType.compareToIgnoreCase(
				ProxyConstants.STARTRECORDINGRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.STARTRECORDINGRESPONSE,
					ProxyConstants.STARTRECORDINGREQUEST, diskSpaceArray, log, db, userJID, db,
					xmpp, resourceFactory, serviceAgentFactory);
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.STOPRECORDINGRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.STOPRECORDINGRESPONSE,
					ProxyConstants.STOPRECORDINGREQUEST, diskSpaceArray, log, db, userJID, db,
					xmpp, resourceFactory, serviceAgentFactory);
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.PAUSERECORDINGRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.PAUSERECORDINGRESPONSE,
					ProxyConstants.PAUSERECORDINGREQUEST, diskSpaceArray, log, db, userJID, db,
					xmpp, resourceFactory, serviceAgentFactory);
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.RESUMERECORDINGRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.RESUMERECORDINGRESPONSE,
					ProxyConstants.RESUMERECORDINGREQUEST, diskSpaceArray, log, db, userJID,
					db, xmpp, resourceFactory, serviceAgentFactory);
		} else if (responseType.compareToIgnoreCase(
				ProxyConstants.RECORDINGSTATUSRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.RECORDINGSTATUSRESPONSE,
					ProxyConstants.RECORDINGSTATUSREQUEST, diskSpaceArray, log, db, userJID,
					db, xmpp, resourceFactory, serviceAgentFactory);
		}  else if (responseType.compareToIgnoreCase(
				ProxyConstants.UPDATERECORDINGRESPONSE) == 0) {
			return ProxyClientAppUtils.generateProxyRecordResponseMessage(responseMessage, ProxyConstants.UPDATERECORDINGRESPONSE,
					ProxyConstants.UPDATERECORDINGREQUEST, diskSpaceArray, log, db, userJID,
					db, xmpp, resourceFactory, serviceAgentFactory);
		} else {
			return null;
		}

	}	
	
	
	
	public static StringTokenizer generateAuthorizationUpdateResponse(Document proxyRequestDoc,
			Element mediaStreamFileResource, int errorCode,
			String errorDescription, String id, MRRequest request,
			String userJID, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		// Document doc = processEvent(event);
		Element el = null;
		String requestID = null;
		String agentName = null;
		String user = null;
		String nickname = null;

		String requestName = proxyRequestDoc.getRootElement().getName();
		//System.out.println("In generate response message");
		
		el = MRXMLUtils.getRootElement(proxyRequestDoc);
		requestID = el.element("header").elementText("requestID");

		if ((requestID == null) || requestID.trim().equals("")) {
			requestID = "None";
		}

		agentName = el.element("data").element("requestorInfo")
		.elementText("agentName");

		if ((agentName == null) || agentName.trim().equals("")) {
			agentName = "None";
		}

		user = el.element("data").element("requestorInfo").elementText(
		"JID");

		if ((user == null) || user.trim().equals("")) {
			user = "None";
		}

		nickname = el.element("data").element("requestorInfo")
		.elementText("nickName");

		if ((nickname == null) || nickname.trim().equals("")) {
			nickname = "None";
		}
		
		if (requestName.compareToIgnoreCase(
				ProxyConstants.AUTHORIZATIONUPDATEREQUEST) == 0) {
			String sourceAgentName = el.element("data").element("StreamData")
			.element("sourceInfo").elementText("agentName");

			if ((sourceAgentName == null) || sourceAgentName.trim().equals("")) {
				sourceAgentName = "None";
			}

			String sourceJID = el.element("data").element("StreamData")
			.element("sourceInfo").elementText("JID");

			if ((sourceJID == null) || sourceJID.trim().equals("")) {
				sourceJID = "None";
			}

			String sourceNickName = el.element("data").element("StreamData")
			.element("sourceInfo").elementText("nickName");

			if ((sourceNickName == null) || sourceNickName.trim().equals("")) {
				sourceNickName = "None";
			}

			String destAgentName = el.element("data").element("StreamData")
			.element("destInfo").elementText("agentName");

			if ((destAgentName == null) || destAgentName.trim().equals("")) {
				destAgentName = "None";
			}

			String destJID = el.element("data").element("StreamData")
			.element("destInfo").elementText("JID");

			if ((destJID == null) || destJID.trim().equals("")) {
				destJID = "None";
			}

			String destNickName = el.element("data").element("StreamData")
			.element("destInfo").elementText("nickName");

			if ((destNickName == null) || destNickName.trim().equals("")) {
				destNickName = "None";
			}

			String connect = el.element("data").element("StreamData")
			.element("profileInfo").elementText("connect");

			if ((connect == null) || connect.trim().equals("")) {
				connect = "None";
			}

			String remote = el.element("data").element("StreamData")
			.element("profileInfo").elementText("remote");

			if ((remote == null) || remote.trim().equals("")) {
				remote = "None";
			}

			return (new StringTokenizer("send " + ProxyConstants.SEPERATOR
					+ "authorizationupdateresponse " + ProxyConstants.SEPERATOR
					+ requestID + ProxyConstants.SEPERATOR + agentName
					+ ProxyConstants.SEPERATOR + user
					+ ProxyConstants.SEPERATOR + nickname
					+ ProxyConstants.SEPERATOR + sourceAgentName
					+ ProxyConstants.SEPERATOR + sourceJID
					+ ProxyConstants.SEPERATOR + sourceNickName
					+ ProxyConstants.SEPERATOR + destAgentName
					+ ProxyConstants.SEPERATOR + destJID
					+ ProxyConstants.SEPERATOR + destNickName
					+ ProxyConstants.SEPERATOR + connect
					+ ProxyConstants.SEPERATOR + remote,
					ProxyConstants.SEPERATOR));
		}

		return null;
	}
	
	
	@SuppressWarnings("unchecked")
    private static Document replaceMetaDataInTemplate (Document metaDataTemplateDoc, Document doc, long xmpp_date, String lab, String hospitalName) {
		
		if(metaDataTemplateDoc == null || doc == null) {
			return metaDataTemplateDoc;
		}
		
		try {
			List<Element> tagEl = null;
			tagEl = metaDataTemplateDoc.getRootElement().elements("Tag");
			if(tagEl!=null) {
				for (int i = 0; (tagEl != null) && (i < tagEl.size()); i++) {
					Element El1 = tagEl.get(i);
					String tagName = MRXMLUtils.getAttributeValue(El1,"Name");
					if("StartTimeOffset".equalsIgnoreCase(tagName)) {
						tagName = "TimeOffset";
					}
					
					String value = MRXMLUtils.getValueXpath(doc, "//" + tagName, "");
					if("Date".equalsIgnoreCase(tagName)) {
						value = String.valueOf(xmpp_date);
					}
					if("Lab".equalsIgnoreCase(tagName)) {
						value = lab; 
					}
					if("Hospital".equalsIgnoreCase(tagName)) {
						value = hospitalName; 
					}
					
					if(!"".equals(value)) {
						El1.setText(value);
					}	
				}
			}
			return metaDataTemplateDoc;
		}catch(Exception e){
			e.printStackTrace();
		}
		return metaDataTemplateDoc;
		
	}
	
	public static Element generateMetaData(Element el, Element metaDataEl, long xmpp_date, MRDB db, MRLog log, MRRequest request) throws Exception {
		
		Document metaDataTemplateDoc = MRXMLUtils.loadXMLResource("/com/ipvs/proxy/impl/MetaDataInstance.xml", log);
		Element recordInfo = MRXMLUtils.getElementXpath(el, "//recordInfo");
		String lab = ProxyClientAppUtils.getSourceTitle();
		String hospital = ProxyClientAppUtils.getHospitalName(log, db, request);
		metaDataTemplateDoc = replaceMetaDataInTemplate(metaDataTemplateDoc,MRXMLUtils.elementToDocument(recordInfo),xmpp_date, lab, hospital);
		//System.out.println(MRXMLUtils.documentToString(metaDataTemplateDoc));
		return metaDataTemplateDoc.getRootElement();
	}
	
	@SuppressWarnings("unchecked")
    public static String getHospitalName(MRLog log, MRDB db, MRRequest request) throws Exception {
		String title = ProxyClientAppUtils.getSourceTitle();
		  String [] xpath = {"//MediaStreamSrcPortResource[//Title='" + title + "']"};
		 Element response = MRXMLUtils.newElement("ResponseXML");
		 String hospitalName = "";
		 
		MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, "MediaStreamSrcPortResource", log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element mediaStreamSrcPortResourceEL = MRXMLDBUtils.readElement(1, null, xpath, parentNID, 1, response, null, request, db, log);
        //System.out.println(MRXMLUtils.elementToString(mediaStreamSrcPortResourceEL));
        if(mediaStreamSrcPortResourceEL != null) {
        	Element groupsEl = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(mediaStreamSrcPortResourceEL), "//Info/Groups");
        	if(groupsEl != null) {
        		List<Element> tagList = groupsEl.elements("Tag");
        		for(int i=0;tagList!=null && i < tagList.size();i++) {
        			Element tagEl = tagList.get(i);
        			if(MRXMLUtils.getAttributeValue(tagEl,"Name").equalsIgnoreCase("Location")) {
        				hospitalName = MRXMLUtils.getAttributeValue(tagEl,"Value");
        			}
        		}
        	}
        }
        return hospitalName;
	}
	
	

	public static void sendStartRecordStream(String streamNID,
			Element profileXMLEL, MRRequest request, String userJID,
			XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String profileNID = MRXMLUtils.getAttributeValue(profileXMLEL, "NID",
		"");

		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH
				+ "AddStreamActionRequest.xml", log);
		MRXMLUtils.setValueXpath(requestDoc, "//profileNID", profileNID);

		String requestName = "AddStreamActionRequest";

		if (profileXMLEL != null) {
			MRXMLUtils.getElementXpath(requestDoc, "//profileXML").add(
					profileXMLEL.detach());
		}

		MRXMLUtils.setValueXpath(requestDoc, "//streamNID", streamNID);
		MRXMLUtils.setValueXpath(requestDoc, "//profileNID", profileNID);

		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	}

		
	public static void sendStopRecordingRequest(String roomNID,
			MRRequest request, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		deleteMediaRoom(roomNID, request, log, db, xmpp, resourceFactory,
				serviceAgentFactory);
		recordingSession.setSessionState(ProxySession.WAIT_FOR_DELETE_MEDIA_ROOM_RESPONSE);
	}

	public static void deleteMediaRoom(String roomNID, MRRequest request,
			MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String deleteMediaRoomXML = "DeleteMediaRoomRequest.xml";
		String requestName = "DeleteMediaRoomRequest";
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH + deleteMediaRoomXML, log);
		MRXMLUtils.setValueXpath(requestDoc, "//roomNID", roomNID);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	}

	public static void sendRecordingPauseResumeRequest(String streamNID,
			String action, MRRequest request, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String addTrackActionRequestXML = "AddTrackActionRequest.xml";
		String requestName = "AddTrackActionRequest";
		Document requestDoc = MRXMLUtils
		.loadXMLResource(ProxyConstants.MEDIAROOM_XMLFILEPATH
				+ addTrackActionRequestXML, log);
		MRXMLUtils.setValueXpath(requestDoc, "//streamNID", streamNID);
		MRXMLUtils.setValueXpath(requestDoc, "//updateAction", action);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
		recordingSession.setSessionState(ProxySession.WAIT_FOR_ADD_TRACK_ACTION_RESPONSE);
	}

	public static boolean isStreamStarted(String recordID, String requestName,
			String streamNID) {
		switch (ProxyRequestNameList.valueOf(requestName.toUpperCase())) {
		case PAUSERECORDINGREQUEST:
		case RESUMERECORDINGREQUEST:
		case CREATEBOOKMARKREQUEST:
		case STOPRECORDINGREQUEST:

			if ((streamNID == null)) {
				return true;
			}

			break;
		}

		return false;
	}


	public static boolean isRecordingRequestsPending(String fileNID, MRLog log)
	throws MRException, Exception {
		
		boolean isRequestPending = false;

		if(recordingSession.getProxyRequestInProgress()!= null) {
			isRequestPending = true;
		}

		if (isRequestPending) {
			recordingSession.setProxyRequestInProgress(null);
			return true;
		}

		return false;
	}

	public static String getRecordIDForRoom(String roomNID) {
		if(roomNID.equalsIgnoreCase(recordingSession.getRoomNID())) {
			return recordingSession.getRecordID();
		}
		else {
			return null;
		}
	}

	public static String getTimeCode(Element mediaStreamFileResource,
			boolean isStart) {
		long dateCreated  = 0;
		try {
			dateCreated = Long.valueOf(MRXMLUtils.getValueXpath(mediaStreamFileResource, "//DateCreated", ""));
		}catch (Exception e) {
			dateCreated = 0;
		}
		
		if(isStart) {
			return String.valueOf(dateCreated);
			
		}
		
		long playlength = 0;
		try{
			playlength = Long.valueOf(MRXMLUtils.getValueXpath(mediaStreamFileResource, "//PlayLength", ""));
		}	
		catch(Exception e) {
			playlength = 0;
		}
		
		long endTc = dateCreated + playlength;
		return String.valueOf(endTc);
			
		
	}

	
	public static String getTimeCode_New(Element mediaStreamFileResource,
			boolean isStart) {
		try {
			Element mediaTrackIndex = mediaStreamFileResource;
			long mediaOffset = 0;
			long endTCLong = 0;

			if (mediaTrackIndex != null) {
				List<?> mediaTrackItemList = mediaTrackIndex.elements("MediaTrackItem");

				for (int i = 0; (mediaTrackItemList != null)
				&& (i < mediaTrackItemList.size()); i++) {

					Element mediaTrackItem = (Element) mediaTrackItemList.get(i);
					Element mediaSegmentInfo = MRXMLUtils.getElementXpath(mediaTrackItem, "MediaSegmentInfo");
					if(mediaSegmentInfo == null) {
						continue;
					}
					long currentMediaOffset = 0;

					try {
						currentMediaOffset = Long.valueOf(MRXMLUtils.getValue(mediaSegmentInfo,"MediaOffset",""));
					} catch (Exception e) {
						currentMediaOffset = 0;
					}

					if (isStart) {
						if (currentMediaOffset == 0) {
							try {
								Element startTC = MRXMLUtils.getElementXpath(
										mediaSegmentInfo, "StartTC");
								long startTCLong = Long.valueOf(MRXMLUtils.getValue(startTC));

								return String.valueOf(startTCLong);
							} catch (Exception e) {
								e.printStackTrace();

								return "0";
							}
						}
					} else {
						try {
							Element endTC = MRXMLUtils.getElementXpath(
									mediaSegmentInfo, "EndTC");

							if (currentMediaOffset >= mediaOffset) {
								endTCLong = Long.valueOf(MRXMLUtils.getValue(endTC));
								mediaOffset = currentMediaOffset;
							}
						} catch (Exception e) {
							e.printStackTrace();

							return "0";
						}
					}
				}

				if (!isStart) {
					return String.valueOf(endTCLong);
				}

				return "0";
			}

			return "0";
		} catch (Exception e) {
			return "0";
		}
	}

	
	
	public static void getRequestorInfo(Element el, String requestID,
			String agentName, String user, String nickname) {
		requestID = el.element("header").elementText("requestID");

		if ((requestID == null) || requestID.trim().equals("")) {
			requestID = "";
		}

		agentName = el.element("data").element("requestorInfo").elementText(
		"agentName");

		if ((agentName == null) || agentName.trim().equals("")) {
			agentName = "";
		}

		user = el.element("data").element("requestorInfo")
		.elementText("JID");

		if ((user == null) || user.trim().equals("")) {
			user = "";
		}

		nickname = el.element("data").element("requestorInfo").elementText(
		"nickName");

		if ((nickname == null) || nickname.trim().equals("")) {
			nickname = "";
		}
	}

	public static String getNickName(String userJID, MRDB db, MRLog log) {
		String nickName = null;

		try {
			MRRequest request = null;
			String requestID = Long.toHexString(ProxyClientUtils.wheel.nextLong());
			String requestName = "getnickname";
			Document doc = null;
			request = MRRequest.createRequest(userJID, MRLog.INFO, requestID,
					"serviceName", requestName, new MRMessage(doc), "");

			Element userEL = DBServiceUtils.getUser(request, db, log);

			if (userEL != null) {
				nickName = MRXMLUtils.getValueXpath(userEL, "Name", "");
			}

			if ((userEL == null) || (nickName == null)
					|| nickName.trim().equals("")) {
				nickName = userJID;
			}
		} catch (Exception e) {
			e.printStackTrace();
			nickName = userJID;
		}

		return nickName;
	}

	public static String getSourceTitle() {
		return sourceTitle;
	}
	

	public static void setSourceTitle(String sourceTitle) {
		ProxyClientAppUtils.sourceTitle = sourceTitle;
	}

	public static Document createResponseDoc(MRRequest request, String status)
    throws MRException, Exception {
    String rdname = request.getResponseDataName();
    Document responseDoc = null;
    StringBuffer responseStr = new StringBuffer();
    responseStr.append("<" + rdname + ">");
    responseStr.append(MRXMLUtils.generateXML("status", status));
    responseStr.append("</" + rdname + ">");
    //System.out.println(responseStr);
    responseDoc = MRXMLUtils.stringToDocument(responseStr.toString());
    
    return responseDoc;
	}
	
	static void sendAddDestFileRequest(MRRequest request, String mediaStorePortResourceNID, String parentMediaDirNID, Element metaDataEL, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String addDestinationFileRequest = "AddDestFileRequest.xml";
		String requestName = "AddDestFileRequest";
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH + addDestinationFileRequest, log);
        MRXMLUtils.setValueXpath(requestDoc, "//FileTitle", "ocprecordingtitle");
        MRXMLUtils.setValueXpath(requestDoc, "//ParentMediaDirNID", parentMediaDirNID);
        MRXMLUtils.setValueXpath(requestDoc, "//MediaStorePortResourceNID", mediaStorePortResourceNID);
        MRXMLUtils.setValueXpath(requestDoc, "//ClipType", MediaRoomServiceConst.MEDIACLIP);
        MRXMLUtils.replaceContent(requestDoc, "//MetaData", metaDataEL);
        Element groupsEL = MRXMLUtils.getElementXpath(requestDoc, "//Groups");
        groupsEL.getParent().remove(groupsEL);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	}
    
	
	private static void sendUpdateRecordingRequest(Element proxyRequestMessageEL,
			String recordID, long serverTime, String userJID, MRRequest request,
			MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String startRecordingXML = "UpdateDestFileMetaDataRequest.xml";
		String requestName = "UpdateDestFileMetaDataRequest";
		String requestString = MRXMLUtils.elementToString(proxyRequestMessageEL);
		String clientdataString = "<![CDATA[" + requestString + "]]>";
		clientdataString = URLEncoder.encode(clientdataString, "UTF-8");
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH + startRecordingXML, log);
		Element metaDataEL = MRXMLUtils.newElement("MetaData");
		MRXMLUtils.removeContent(metaDataEL);
		//int resourceindex = userJID.indexOf("/");
		//String lab = userJID.substring(resourceindex + 1);
		metaDataEL = ProxyClientAppUtils.generateMetaData(proxyRequestMessageEL, metaDataEL, serverTime, db, log, request);
		MRXMLUtils.setValueXpath(requestDoc, "//"+ MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
		Element requestMetaDataEL = MRXMLUtils.getElementXpath(requestDoc,
				"//MetaData");
		MRXMLUtils.replaceContent(requestMetaDataEL, metaDataEL);
		Element clientData = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientData.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(requestDoc, "//header");
		MRXMLUtils.removeChild(headerElement,"clientdata");
		headerElement.add(clientData);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	}
	
	public static void sendGetServerTimeRequest(MRRequest request, XMPPI xmpp, MRLog log, ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String requestString = MRXMLUtils.elementToString(request.getMessage().getDocument().getRootElement());
		String clientdataString = "<![CDATA[" + requestString + "]]>";
		String getServerTimeRequest = "GetServerTimeRequest.xml";
		String requestName = "GetServerTimeRequest";
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.SYSTEM_XMLFILEPATH + getServerTimeRequest, log);
		Element clientData = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientData.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(requestDoc, "//header");
		MRXMLUtils.removeChild(headerElement, "clientdata");
		headerElement.add(clientData);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	}
	
	static void sendCreateConnectionRequest(MRRequest request, String sourceNID, String destNID, Element profileXMLEL, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
			String createConnectionRequest = "CreateConnectionRequest.xml";
	        String profileNID = MRXMLUtils.getAttributeValue(profileXMLEL, "NID", "");
	        String requestName = "CreateConnectionRequest";
	        Document requestDoc = MRXMLUtils.loadXMLResource(ProxyConstants.MEDIAROOM_XMLFILEPATH + createConnectionRequest, log);
	        MRXMLUtils.setValueXpath(requestDoc, "//sourceNID", sourceNID);
	        MRXMLUtils.setValueXpath(requestDoc, "//name", "TestConnection");
	        MRXMLUtils.setValueXpath(requestDoc, "//profileNID", profileNID);
	        MRXMLUtils.getElementXpath(requestDoc, "//profileXML").add(profileXMLEL.detach());
	        if(destNID == null) {
	        	destNID = "";
	        }
	        MRXMLUtils.setValueXpath(requestDoc, "//destNID", destNID);
	        MRXMLUtils.setValueXpath(requestDoc, "//group", "RedGroup");
	        MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
	        
    }
	
	
    public static Document handleXAuthRequest(ProxyClient proxyClient, String userJID, String fromJID, String message, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	
    	log.addInfo(":Received XAuth Request", MRLog.INFO,MRLog.NOTIFY);
    	String streamNID = null;
    	Document doc = null;
    	doc = MRXMLUtils.getDocument(message);
    	double[] diskSpaceArr = {0,0};
    	if (doc != null) {
    		streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
    	}

    	if (streamNID != null) {
    		String requestorJID = null;
    		String remote = "no";
    		boolean authAuth = Boolean.valueOf(MRXMLUtils.getValueXpath(doc, "//autoAuth", "true"));
    		if (doc != null) {
    			requestorJID = MRXMLUtils.getValueXpath(doc, "//requesterJID", "");
    			remote = ProxyClientAppUtils.getEnableKBM(doc);
    			//streamDataEl = MRXMLUtils.getElementXpath(doc, "//streamDataDoc");
    			
    		}
    		log.addInfo("DeviceXAuthServerAgent : Stream->" + streamNID + " Requestor :" + requestorJID, MRLog.INFO,MRLog.NOTIFY);
  			String id = streamNID;
  			if(authAuth) {
  				Document authResponse = ProxyClientAppUtils.generateXAuthResponse(id, userJID, requestorJID, remote, db, log);
  				return authResponse;
  			}
   			StringBuffer proxyMessage = new StringBuffer();
   			proxyMessage.append("send" + ProxyConstants.SEPERATOR + "authorizestreamrequest" + ProxyConstants.SEPERATOR);
    		proxyMessage.append(id + ProxyConstants.SEPERATOR);
    		proxyMessage.append(ProxyClientAppUtils.generateProxyAuthRequestMessage(userJID, requestorJID, "no", remote, db, log));
    		StringTokenizer st = ProxyClientAppUtils.generateResponse(proxyMessage.toString(), ProxyConstants.AUTHREQUEST, userJID, diskSpaceArr, log, db, xmpp, resourceFactory, serviceAgentFactory);
    		proxyClient.sendResponse(st);
    	}

    	return null;
    }
    
    public static Document generateXAuthResponse(String requestID, String sourceAgentJID, String destAgentJID,  String remote, MRDB db, MRLog log) {
    	String xmlFilePath = ProxyConstants.XMLFILEPATH + "authorizestreamresponse.xml";
    	Document responsedoc = null;
    	try {
	    	String sourceAgentName = ServiceAgentUtils.getAgentName(sourceAgentJID);
    		String sourceAgentNickName = getNickName(sourceAgentJID, db, log);
    		String destAgentName = ServiceAgentUtils.getAgentName(destAgentJID);
	    	String destAgentNickName = getNickName(destAgentJID, db, log);
    		responsedoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
    		Element parentEL = responsedoc.getRootElement();
    		MRXMLUtils.setValueXpath(parentEL, "//requestID", requestID);
    		MRXMLUtils.setValueXpath(parentEL, "//requestorInfo/agentName", destAgentName);
 	        MRXMLUtils.setValueXpath(parentEL, "//requestorInfo/JID", destAgentJID);
 	        MRXMLUtils.setValueXpath(parentEL, "//requestorInfo/nickName", destAgentNickName);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/sourceInfo/agentName", sourceAgentName);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/sourceInfo/JID", sourceAgentJID);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/sourceInfo/nickName", sourceAgentNickName);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/destInfo/agentName", destAgentName);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/destInfo/JID", destAgentJID);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/destInfo/nickName", destAgentNickName);
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/profileInfo/connect", "Yes");
 	        MRXMLUtils.setValueXpath(parentEL, "//streamData/profileInfo/remote", remote);
 	        MRXMLUtils.setValueXpath(parentEL, "//Result", "Allow");
    	}catch(Exception e) {
    		e.printStackTrace();
    	}
    	return responsedoc;
    }
    
    public static void handleStreamStart(Document notifyMessageDoc, String action,
			String nodeID, MRDB schema, ProxyClient proxyClient, MRLog log, 
			String userJID, XAuthServerAgent xAuthAgent , XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
    			log.addInfo(":handleStreamStart", MRLog.INFO,MRLog.NOTIFY);
				String streamNID = MRXMLUtils.getValueXpath(notifyMessageDoc, "//streamNID", "");
				String startTime = MRXMLUtils.getValueXpath(notifyMessageDoc, "//startTime", "0");
				xAuthAgent.setStartTime(streamNID, startTime);
				Document streamData = xAuthAgent.getXAuthEntry(streamNID);
	            if ((streamData == null)) {
	                return;
	            }
				String authNotifyMessage = ProxyClientAppUtils.generateProxyAuthNotifyMessage(streamData, null, ProxyConstants.STATE_CONNECTED, log, schema);
				ProxyClientAppUtils.sendProxyResponseMessage(ProxyConstants.AUTHNOTIFY,authNotifyMessage, userJID, proxyClient, log, schema, xmpp, resourceFactory, serviceAgentFactory);
				xAuthAgent.updateState(MediaRoomServiceConst.STREAM_STARTED, streamNID, null);
    }

	public static ProxySession getRecordingSession() {
		return recordingSession;
	}

	public static void setRecordingSession(ProxySession recordingSession) {
		ProxyClientAppUtils.recordingSession = recordingSession;
	}
    
}
