package com.ipvs.sessionservice.session.handler;

import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
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
import com.ipvs.policyadminservice.impl.SessionPolicyUtils;
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class StartSessionRequestHandler implements MRRequestHandlerI,EventLoggerI{

	public static final String GETDESTMEDIADIRLISTREQUESTXML = "/com/ipvs/mediaroomservice/xml/GetDestMediaDirListRequest.xml";
	public static final String GETPVRDESTMEDIADIRLISTREQUESTXML = "/com/ipvs/mediaroomservice/xml/GetPVRDestMediaDirListRequest.xml";
	
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

	
	public Document startSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		request.setCache(new MRRequestCache());
		Document doc = request.getMessage().getDocument();
		// System.out.println("SessionRequestHandler doc=" + MRXMLUtils.documentToString(doc));
		// String sessionID = MRXMLUtils.getElementXpath(doc, "//StartSession").getAttributeValue("SessionId");
		String sessionID = MRXMLUtils.getAttributeValueXpath(doc, "//" +SessionServiceConst.STARTSESSION,"sessionId",null);
		// Check if session exists
		if(AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionID) != null) {
    		throw new MRException(SessionServiceExceptionCodes.DUPLICATE_SESSION,"Duplicate SessionId=" + sessionID);
		}
        String sessionConfigNID = MRXMLUtils.getAttributeValueXpath(doc, "//Properties","sessionConfigNID",null);

		// String sessionID = MRXMLUtils.getElementXpath(doc, "//SessionStart").getAttributeValue("sessionId");

		if(sessionID == null || "".equals(sessionID))
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "");
		Element session = MRXMLUtils.getElementXpath(doc, "//" + SessionServiceConst.SESSION);
		MRXMLUtils.addAttribute(session,SessionServiceConst.ID, sessionID);
		Document sessionDoc = MRXMLUtils.elementToDocument(session);

		//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
		XMPPTransaction.startBlock("StartSessionRequestHandler");
		
		XMPPTransaction.startBlock("UpdateSession");
		SessionResourceUtils.updateSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, sessionID, sessionConfigNID);
		XMPPTransaction.endBlock("UpdateSession");
		
		SessionPolicyUtils.enforcePolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
				
		XMPPTransaction.startBlock("CreateSession");
		String roomNID = SessionMediaRoomUtils.createSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
		XMPPTransaction.endBlock("CreateSession");
		List<?> entityList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY);
		
		
		
		XMPPTransaction.startBlock("AddRoomAndEntities");		
		SessionMediaRoomUtils.createSourcesAndDestinations(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, roomNID, entityList);
		XMPPTransaction.endBlock("AddRoomAndEntities");
		
		
		
		XMPPTransaction.startBlock("startConnectionList");		
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.CONNECTION);
		SessionMediaRoomUtils.startConnectionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				connectionList, sessionID);
		XMPPTransaction.endBlock("startConnectionList");
		
		
		XMPPTransaction.endBlock("StartSessionRequestHandler");
		//XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
		
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDocWithMediaRoomInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).handleSessionStarted(xmpp, request, log, db, roomNID);
		
		// Resume from Start if this is Sync Playback so that the streams are sync'd up
		this.resumeSyncPlaybackFromStart(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, sessionID);
		
		return request.createResponseData((Element)sessionDoc.getRootElement().detach());

	}	

	public void resumeSyncPlaybackFromStart(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String sessionID){
		
		try {
			String sessionType = sessionDoc.getRootElement().attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);
			if(!SessionServiceConst.PLAYBACK.equals(sessionType))
				return;
			Element playbackGroupFileElement = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.GRPFILE + "[@type='" + SessionServiceConst.SYNCPLY + "']");
			if(playbackGroupFileElement == null)
				return;
	 		String syncEntityId = MRXMLUtils.getAttributeValue(playbackGroupFileElement,SessionServiceConst.SYNCENTITYID);
			if( (syncEntityId == null) || "".equals(syncEntityId) )
				return;
				
			List<?> entityList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type='" + SessionServiceConst.PLYFILE + "']");
			if(entityList.size() <= 1)
				return;
			
			log.addInfo("StartSessionRequestHandler:resumeSyncPlaybackFromStart", MRLog.OK, MRLog.NOTIFY);

			// This is multi stream sync playback
			ResumeRequestHandler resumeRequestHandler = new ResumeRequestHandler();
			Document resumeRequestDoc = MRXMLUtils.stringToDocument("<Resume/>");
			resumeRequestDoc.getRootElement().addAttribute(SessionServiceConst.SESSION_ID, sessionID);
			resumeRequestDoc.getRootElement().addAttribute(SessionServiceConst.STARTOFFSET, "0");
			MRRequest resumeRequest = MRRequest.createRequest(request, resumeRequestDoc, 
					request.getRequestName(), request.getServiceName());
			resumeRequestHandler.getResponse(resumeRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		catch(Exception e) {
			log.addInfo("StartSessionRequestHandler:resumeSyncPlaybackFromStart exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
			e.printStackTrace();			
		}
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document doc = request.getMessage().getDocument();
		// System.out.println("SessionRequestHandler doc=" + MRXMLUtils.documentToString(doc));
		// String sessionID = MRXMLUtils.getElementXpath(doc, "//StartSession").getAttributeValue("SessionId");
		String sessionId = MRXMLUtils.getAttributeValueXpath(doc, "//" +SessionServiceConst.STARTSESSION,"sessionId",null);
		
		Document response = null;
		Exception exp = null;					
		Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());				
        log.addInfo("StartSessionRequestHandler:getResponse got lock for=" + request.getNID() + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		try {
			response = this.startSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory);			
			
			try {
				
				//log session start event 
				request.addAuditEvent(SessionAuditEventUtils.getSessionAuditEventObject(response, AuditEventLogger.NAME_START, request, AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, log, ""));

				//log connection events
				List<?> connectionList = MRXMLUtils.getListXpath(response, "//Connection");
				for(Object object:connectionList) {					
					AuditEvent connectionAuditEvent = SessionAuditEventUtils.getSourceBasedAuditEventUsingConnectionElement(response, (Element) object, request, log);
					if(connectionAuditEvent !=null){
						request.addAuditEvent(connectionAuditEvent);
					}
				}
				
			} catch (Exception e) {
				//ignore exception
				e.printStackTrace();
			}
		}
		catch(Exception e) {
			exp = e;
		}
		IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
        log.addInfo("StartSessionRequestHandler:getResponse released lock for=" + request.getNID() + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		if(exp != null)
			throw exp;

		return response;
	}


	
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, String status, String level, String errorDescription)	throws Exception {
		
		//if start session failed then only log event from here otherwise get response code will log the event
		if(request.getException()!=null) {
			return SessionAuditEventUtils.getSessionAuditEventObject(request.getMessage().getDocument(), AuditEventLogger.NAME_START, request, status, level, log, errorDescription);
		}
		
		return null;
	}
}
