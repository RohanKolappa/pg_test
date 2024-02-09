package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
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
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class SwitchSrcRequestHandler implements MRRequestHandlerI{

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
		request.setCache(new MRRequestCache());
		Document doc = request.getMessage().getDocument();
		Element switchSrcElement = MRXMLUtils.getElementXpath(doc, "//SwitchSrc");
		String sessionId = switchSrcElement.attributeValue("sessionId", null);
		String connId = switchSrcElement.attributeValue("connId", null);
		String newSrcEntityId = switchSrcElement.attributeValue("newSrcEntityId", null);
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sessionId);
		
		//Set logger document in request document
		String sessionNameString =SessionAuditEventUtils.getSessionNameDescriptionString(sessionDoc);
		Document loggerDocument = MRXMLUtils.stringToDocument("<Log sessionName='"+sessionNameString+"'/>");
		Element loggerEl = loggerDocument.getRootElement();
		request.setLoggingDocument(loggerDocument);
		
		Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		if(connection == null) {
			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Connection not found connId=" + connId);
		}
		String connectionType = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.TYPE_ATTRIBUTE,"");

		//if connection exist then add previous source and destination information in logger
		MRXMLUtils.setAttributeValue(loggerEl, "previousSource", SessionAuditEventUtils.getSourceTitle(sessionDoc, connection, request, log));
		MRXMLUtils.setAttributeValue(loggerEl, "destinationDescString", SessionAuditEventUtils.getDestinationDescriptionString(sessionDoc, connectionType,connection));	
		
		SessionMediaRoomUtils.validateSrcEntityId(sessionDoc, newSrcEntityId);
		
		//add new source information in logger document		
		Element newSrcEntityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + newSrcEntityId + "']");	 
		String elementName = SessionServiceConst.PORT;
		if(MRXMLUtils.getAttributeValue(newSrcEntityEl, SessionServiceConst.TYPE_ATTRIBUTE,"").equals(SessionServiceConst.PLYFILE)) {
			elementName = SessionServiceConst.FILE;
		}
		MRXMLUtils.setAttributeValue(loggerEl, "newSource", SessionAuditEventUtils.getSourceTitle(MRXMLUtils.getElement(newSrcEntityEl, elementName)));
		
		if(!SessionResourceUtils.isUsedInConnection(sessionDoc, newSrcEntityId)) { // Add the mediaSource
			String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
			SessionMediaRoomUtils.createMediaSource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					newSrcEntityId, sessionDoc, roomNID);
		}
		
		String newDstEntityId = connection.attributeValue(SessionServiceConst.DSTENTITYID);
		String connectionName = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.NAME,"");
		String canvasId = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.CANVASID,"");
		String windowId = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.WINDOWID,"");
		String profileId = ""; // Profile Id not supported in SwitchSrc
		
		connection = SessionMediaRoomUtils.switchConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				connId, newSrcEntityId, newDstEntityId, sessionId, connection,connectionType,connectionName,canvasId,windowId,profileId);
		
		return request.createResponseData((Element)connection.detach());
	}
}
