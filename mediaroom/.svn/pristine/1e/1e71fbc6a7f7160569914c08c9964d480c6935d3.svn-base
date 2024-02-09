package com.ipvs.sessionservice.session.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class DeleteConnRequestHandler implements MRRequestHandlerI {

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
		
		Document doc = request.getMessage().getDocument();
		Element deleteConnElement = MRXMLUtils.getElementXpath(doc, "//DeleteConn");
		String sessionId = deleteConnElement.attributeValue("sessionId", null);
		String connId = deleteConnElement.attributeValue("connId", null);
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, ".//Connection[@type!='"+SessionServiceConst.PVR+"']");
		String gc = MRXMLUtils.getAttributeValueXpath(sessionDoc.getRootElement(), "//Session/@gc",MRConst.FALSE);
		boolean stopSession = (Boolean.parseBoolean(gc) && connectionList.size() == 1);
		if (stopSession){
			//logging stop session internally
			SessionMediaRoomUtils.stopViewSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, doc, sessionId);			
		} else {
			SessionMediaRoomUtils.deleteConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				connId, sessionId);
			
			//log delete connection event
			Element connectionElement = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
			Element srcElement =SessionAuditEventUtils.getSourceEntityElementFromConnectionElement(sessionDoc, connectionElement, request, log);
			request.addAuditEvent(SessionAuditEventUtils.getAuditEventBasedOnSourceAndDestination(sessionDoc, connectionElement, AuditEventLogger.NAME_STOP, srcElement, 
					AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO,"", request, log));
					
		 }
		return request.createResponseData((Element)deleteConnElement.clone());
	}
}
