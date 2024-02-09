package com.ipvs.sessionservice.session.handler;

import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.SetPortAutoStartRequestHandler;
import com.ipvs.sessionservice.resource.handler.SetPortPVRInfoRequestHandler;

public class StopSessionRequestHandler implements MRRequestHandlerI,EventLoggerI {
	private final String SET_PORT_PVR_INFO_REQUEST_XML = "/com/ipvs/sessionservice/resource/xml/SetPortPVRInfoRequest.xml";
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

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document doc = request.getMessage().getDocument();	
		String sessionId = MRXMLUtils.getAttributeValueXpath(doc, "//StopSession","sessionId", null);	
		log.addInfo("StopSessionRequestHandler.getResponse: Acquired lock on Session ID: "+sessionId);
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String sessionType = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE, "");
		// Since Session name might change in future, we are checking for sessionType = "SYSTEM" and ConnectionType ="PVR" for PVR sessions
		if(sessionType.equals(SessionServiceConst.TYPE_SYSTEM)) {
			List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//"+SessionServiceConst.CONNECTION);
			if(connectionList != null && connectionList.size() == 1) {
				Element connectionEl = (Element) connectionList.get(0);
				String connectionType = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
				if(connectionType.equals(SessionServiceConst.PVR)) {
					return changeConfigOnStopSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionType, sessionDoc, connectionEl,SET_PORT_PVR_INFO_REQUEST_XML,"SetPortPVRInfo",SetPortPVRInfoRequestHandler.class.getName());
				}
				//				else if(connectionType.equals(SessionServiceConst.VIEW)){
				//					return changeConfigOnStopSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionType, sessionDoc, connectionEl, SessionServiceConst.SET_PORT_AUTO_START_REQUEST_XML,"SetPortAutoStart",SetPortAutoStartRequestHandler.class.getName());
				//				}
			}
		} else  if(sessionType.equals(SessionServiceConst.TYPE_SYSTEM_READ_ONLY)) {
			throw new MRException(SessionServiceExceptionCodes.CAN_NOT_DELETE_READYONLY_SESSIONS,"Can not delete Read only Sessions");
		}
		return AppServerAgentUtils.getSessionManager(serviceAgentFactory).handleStopSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);


	}

	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String status, String level, String errorDescription)
					throws Exception {
		return SessionAuditEventUtils.getSessionAuditEventObject(request.getResult(), AuditEventLogger.NAME_STOP, request, status, level, log, errorDescription);
	}

	private Document changeConfigOnStopSession(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, String connectionType,
			Document sessionDoc, Element connectionEl, String requestXMLPath, String childElementName, String requestHandlerName) throws Exception{

		String srcEntityId = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.SRCENTITYID, "");
		Element entityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityId+"']");
		String resourceNID = MRXMLUtils.getAttributeValueXpath(entityEl, "./Port", "resourceNID", "");
		Document setPortSampleRequestDoc = MRXMLUtils.loadXMLResource(requestXMLPath, log);		

		Element	SetPortEL = MRXMLUtils.getElementXpath(setPortSampleRequestDoc, "//"+childElementName);

		MRXMLUtils.setAttributeValue(SetPortEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, resourceNID);
		MRXMLUtils.setAttributeValue(SetPortEL, SessionServiceResourceConst.ENABLE_ATTRIBUTE, SessionServiceConst.FALSE);
		String requestName = MRRequest.getRequestName(requestHandlerName);
		Document setPortRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, SetPortEL);
		MRRequest setPortRequest = MRRequest.createRequest(request, setPortRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);	
		if(connectionType.equalsIgnoreCase(SessionServiceConst.PVR)){

			SetPortPVRInfoRequestHandler portPVRInfoRequestHandler = new SetPortPVRInfoRequestHandler();
			portPVRInfoRequestHandler.getResponse(setPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		else{
			Document srcPortDoc = MRXMLDBUtils.readElement(request, log, db,resourceNID);			
			String mode = MRXMLUtils.getAttributeValueXpath(srcPortDoc, "//Instance", "mode", SessionServiceConst.MODE_DYNAMIC);
			if(mode.equalsIgnoreCase(SessionServiceConst.MODE_AUTO_START))
			{
				SetPortAutoStartRequestHandler portAutoStartRequestHandler = new SetPortAutoStartRequestHandler();
				portAutoStartRequestHandler.getResponse(setPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
			}
		}
		return request.createResponseData(sessionDoc.getRootElement());
	}

}
