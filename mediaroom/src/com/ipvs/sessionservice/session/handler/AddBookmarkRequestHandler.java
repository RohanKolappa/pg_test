package com.ipvs.sessionservice.session.handler;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.BookmarkBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class AddBookmarkRequestHandler extends BookmarkBaseRequestHandler {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

	    int authType = MediaRoomObject.ROOM_TYPEID;
	    int role = MediaRoomPEM.VISITOR_ROLE;
	    String token = "testCommonToken";
	    String authority = "testAuthority";
	    String roomPolicyNID = MRConst.NULL;
	    String streamPolicyNID = MRConst.NULL;
	    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
	                authority);
	    log.addTraceEnd(request);

	    return auth;
	}

     
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	Document requestDoc = request.getMessage().getDocument();
    	Element requestEL = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.ADD_BOOKMARK);
    	Element bookmarkEL = MRXMLUtils.getElement(requestEL, SessionServiceConst.BOOKMARK);
        String requestedTC = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.TC, "");
        String requestedOffset = MRXMLUtils.getAttributeValue(bookmarkEL, SessionServiceConst.OFFSET, "");        
        handleNewBookmark(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestEL, requestedTC, requestedOffset, bookmarkEL, SessionServiceConst.ADD);
    	return null;
    }
    
    
	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document agentResponseDoc = request.getResult();
		String clientData = MRXMLUtils.getValueXpath(agentResponseDoc, "//" + MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc, MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		MRException exception = null;
		String logData = "";
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			int exceptionCode = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
			String description = MRXMLUtils.getValueXpath(agentResponseDoc, "//Description", "AgentResponseFailed");
			exception = new MRException(exceptionCode, description);
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exceptionCode), "Description", description);
		}
		
		// create user request
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);

		if (request.getRequestName().equalsIgnoreCase("GetOffsetTimecodeRequest")) {
			
			Element data = MRXMLUtils.getElementXpath(agentResponseDoc, "//Data");

			String tc = MRXMLUtils.getAttributeValue(data, SessionServiceConst.AT_TC);
			String offset = MRXMLUtils.getAttributeValue(data, SessionServiceConst.BOOKMARK_OFFSET);
			String NID = MRXMLUtils.getAttributeValue(data, SessionServiceConst.BY_FILE_NID);

			Document bookmarkDoc = MRXMLDBUtils.readElement(userRequest, log, db, NID);
			String mediaStreamFileNID = MRXMLUtils.getValueXpath(bookmarkDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");

			if (!tc.isEmpty() && tc.length() > 0)
				MRXMLUtils.getElementXpath(bookmarkDoc, "//" + SessionServiceConst.AT_TC).setText(tc);

			if (!offset.isEmpty() && offset.length() > 0)
				MRXMLUtils.getElementXpath(bookmarkDoc, "//" + SessionServiceConst.BOOKMARK_OFFSET).setText(offset);

			String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.BOOKMARK_FILE_INFO + SessionServiceConst.XQUERYXML;
			String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
			query = query.replace("_XPATHAND_", "'" + mediaStreamFileNID + "'");
			request.setSuperUser(true);
			Document queryResultDoc = GetObjectUtils.executeQuery(query, AMConst.MEDIA_STREAM_FILE_RESOURCE, null, "", "", request, xmpp, log, db, serviceAgentFactory,
					resourceFactory, true);
			Document mediaStreamFileDoc = MRXMLUtils.getDocumentXpath(queryResultDoc, "//MediaStreamFileResource");
			String mediaUUID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//UUID", "");
			String mediaGroupId = MRXMLUtils.getValueXpath(queryResultDoc, "//GroupID", "");
			String groupParentDirNID = MRXMLUtils.getValueXpath(queryResultDoc, "//groupParentDirNID", "");
			
			Element portEle = MRXMLUtils.getElementXpath(queryResultDoc, "//groupPortEle");
			Element mediaStorePortElement = MRXMLUtils.getElementXpath(portEle, "//" + AMConst.MEDIA_STORE_PORT_RESOURCE_NID);
			String mediaStreamFileDirID = MRXMLUtils.getAttributeValue(mediaStorePortElement, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, "");

			Document addBookmarkDoc = getBookmarkRequestDocument(bookmarkDoc, groupParentDirNID, log, mediaUUID, mediaStreamFileDirID, mediaGroupId,
					SessionServiceConst.ADD_BOOKMARK_REQUEST);
			sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, addBookmarkDoc, mediaStorePortElement,
					SessionServiceConst.ADD_BOOKMARK_REQUEST);
			return;
		}

		Document userResponseDoc = createOrUpdateBookmarkInDB(userRequest, agentResponseDoc, userRequest.getMessage().getDocument(), true, SessionServiceConst.ADD_BOOKMARK, xmpp,
				log, db, resourceFactory, serviceAgentFactory);		
		userRequest.setResult(userResponseDoc);
		userRequest.setState(state);
		userRequest.setException(exception);
		userRequest.addAuditEvent(this.getAuditEvent(userRequest, xmpp, log, db, AuditEventUtils.getAuditEventStatus(userRequest), AuditEventUtils.getAuditEventLevel(userRequest),
				AuditEventUtils.getAuditEventErrorDescription(userRequest)));
		responseHandler.handleResponse(userRequest, logData);
	}
    
  
}